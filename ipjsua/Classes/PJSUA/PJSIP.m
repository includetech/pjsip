
//  PJSIP.m
//  ipjsua
//
//  Created by include tech. on 25/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import "PJSIP.h"
#define THIS_FILE "PJSUA"
#define current_acc	pjsua_acc_get_default()

static pjsua_app_cfg_t app_cfg;
static pj_bool_t default_mod_on_rx_request(pjsip_rx_data *rdata);
static void on_call_state(pjsua_call_id call_id, pjsip_event *e);
static void on_call_media_state(pjsua_call_id call_id);
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
static void on_call_tsx_state(pjsua_call_id call_id, pjsip_transaction *tsx, pjsip_event *e);
static void call_on_dtmf_callback(pjsua_call_id call_id, int dtmf);
static pjsip_redirect_op call_on_redirected(pjsua_call_id call_id, const pjsip_uri *target, const pjsip_event *e);
static void on_reg_state(pjsua_acc_id acc_id);
static void on_incoming_subscribe(pjsua_acc_id acc_id,
                                  pjsua_srv_pres *srv_pres,
                                  pjsua_buddy_id buddy_id,
                                  const pj_str_t *from,
                                  pjsip_rx_data *rdata,
                                  pjsip_status_code *code,
                                  pj_str_t *reason,
                                  pjsua_msg_data *msg_data);

static void on_call_transfer_status(pjsua_call_id call_id,
                                    int status_code,
                                    const pj_str_t *status_text,
                                    pj_bool_t final,
                                    pj_bool_t *p_cont);

static void on_call_replaced(pjsua_call_id old_call_id, pjsua_call_id new_call_id);
static void on_nat_detect(const pj_stun_nat_detect_result *res);
static void on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info);
static void on_transport_state(pjsip_transport *tp, pjsip_transport_state state, const pjsip_transport_state_info *info);
static pj_status_t on_snd_dev_operation(int operation);
static void on_call_media_event(pjsua_call_id call_id,
                                unsigned med_idx,
                                pjmedia_event *event);


@implementation PJSIP
{
    pjsua_acc_id _acc_id;
}

/*
 Create a Shared Instance
 */

+ (PJSIP *)sharedPJSIP {
    
    static PJSIP *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[PJSIP alloc] init];
    });
    
    return sharedInstance;
}

- (pj_status_t)addAccountWithUsername:(NSString *)username password:(NSString *)password toServer:(NSString *)domain
{
    pj_status_t status = 0;
    status = [[PJSIP sharedPJSIP] startPJSIP];
    
    if(status == 0) {
        pjsua_acc_config acc_cfg;
        pjsua_acc_config_default(&acc_cfg);
        
        acc_cfg.vid_cap_dev = PJMEDIA_VID_DEFAULT_CAPTURE_DEV;
        acc_cfg.vid_rend_dev = PJMEDIA_VID_DEFAULT_RENDER_DEV;
        acc_cfg.vid_in_auto_show = PJ_TRUE;
        acc_cfg.vid_out_auto_transmit = PJ_TRUE;
        
        char *sipUser = [username UTF8String];
        char *sipDomain = [domain UTF8String];
        char *pwd = [password UTF8String];
        
        // Account ID
        char sipId[60];
        sprintf(sipId, "sip:%s@%s", sipUser, sipDomain);
        acc_cfg.id = pj_str(sipId);
        
        // Reg URI
        char regUri[100];
        
        NSString * tpStr = @";transport=TCP";
        sprintf(regUri, "sip:%s%s", sipDomain,[tpStr UTF8String]);
        acc_cfg.reg_uri = pj_str(regUri);
        
        acc_cfg.cred_count = 1;
        acc_cfg.cred_info[1].scheme = pj_str("digest");
        acc_cfg.cred_info[1].realm = pj_str("107.170.46.82");
        acc_cfg.cred_info[1].username = pj_str(sipUser);
        acc_cfg.cred_info[1].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        acc_cfg.cred_info[1].data = pj_str(pwd);
        
        status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &_acc_id);
        
        return status;
    } else {
        return status;
    }
}

- (pj_status_t)startPJSIP {
    pj_status_t status = 0;
    
    pj_bzero(&app_cfg, sizeof(app_cfg));

    status = app_init();
    return  status;
}

static pj_status_t app_init()
{
    pjsua_transport_id transport_id = -1;
    pjsua_transport_config tcp_cfg;
    unsigned i;
    pj_pool_t *tmp_pool;
    pj_status_t status;
    
    /** Create pjsua **/
    status = pjsua_create();
    if (status != PJ_SUCCESS)
        return status;
    
    pjsua_media_config_default(&app_config.media_cfg);
    
    loadDefault_config();
    app_config.media_cfg.vid_preview_enable_native = PJ_TRUE;

    app_config.vid.vcapture_dev = PJMEDIA_VID_DEFAULT_CAPTURE_DEV;
    app_config.vid.vrender_dev = PJMEDIA_VID_DEFAULT_RENDER_DEV;
    app_config.vid.in_auto_show = PJ_TRUE;
    app_config.vid.out_auto_transmit = PJ_TRUE;
    
    /* Create pool for application */
    app_config.pool = pjsua_pool_create("pjsua-app", 1000, 1000);
    tmp_pool = pjsua_pool_create("tmp-pjsua", 1000, 1000);;
    
//    /* Initialize application callbacks */
    app_config.cfg.cb.on_call_state = &on_call_state;
    app_config.cfg.cb.on_call_media_state = &on_call_media_state;
    app_config.cfg.cb.on_incoming_call = &on_incoming_call;
    app_config.cfg.cb.on_call_tsx_state = &on_call_tsx_state;
    app_config.cfg.cb.on_dtmf_digit = &call_on_dtmf_callback;
    app_config.cfg.cb.on_call_redirected = &call_on_redirected;
    app_config.cfg.cb.on_reg_state = &on_reg_state;
    app_config.cfg.cb.on_incoming_subscribe = &on_incoming_subscribe;

    app_config.cfg.cb.on_call_transfer_status = &on_call_transfer_status;
    app_config.cfg.cb.on_call_replaced = &on_call_replaced;
    app_config.cfg.cb.on_nat_detect = &on_nat_detect;
    app_config.cfg.cb.on_mwi_info = &on_mwi_info;
    app_config.cfg.cb.on_transport_state = &on_transport_state;
    app_config.cfg.cb.on_snd_dev_operation = &on_snd_dev_operation;
    app_config.cfg.cb.on_call_media_event = &on_call_media_event;
    
    /* Set sound device latency */
    if (app_config.capture_lat > 0)
        app_config.media_cfg.snd_rec_latency = app_config.capture_lat;
    if (app_config.playback_lat)
        app_config.media_cfg.snd_play_latency = app_config.playback_lat;
    
    if (app_cfg.on_config_init)
        (*app_cfg.on_config_init)(&app_config);
    
    app_config.log_cfg.console_level = 4;
    
    /* Initialize our module to handle otherwise unhandled request */
    status = pjsip_endpt_register_module(pjsua_get_pjsip_endpt(),
                                         &mod_default_handler);
    if (status != PJ_SUCCESS)
        return status;
    
    /* Initialize pjsua */
    status = pjsua_init(&app_config.cfg, &app_config.log_cfg,
                        &app_config.media_cfg);
    if (status != PJ_SUCCESS) {
        pj_pool_release(tmp_pool);
        return status;
    }
    
    /* Initialize calls data */
    for (i=0; i<PJ_ARRAY_SIZE(app_config.call_data); ++i) {
        app_config.call_data[i].timer.id = PJSUA_INVALID_ID;
//        app_config.call_data[i].timer.cb = &call_timeout_callback;
    }

    pj_memcpy(&tcp_cfg, &app_config.udp_cfg, sizeof(tcp_cfg));
    
    /* Create AVI player virtual devices */
    if (app_config.avi_cnt) {
#if PJMEDIA_HAS_VIDEO && PJMEDIA_VIDEO_DEV_HAS_AVI
        pjmedia_vid_dev_factory *avi_factory;
        
        status = pjmedia_avi_dev_create_factory(pjsua_get_pool_factory(),
                                                app_config.avi_cnt,
                                                &avi_factory);
        if (status != PJ_SUCCESS) {
            PJ_PERROR(1,(THIS_FILE, status, "Error creating AVI factory"));
            goto on_error;
        }
        
        for (i=0; i<app_config.avi_cnt; ++i) {
            pjmedia_avi_dev_param avdp;
            pjmedia_vid_dev_index avid;
            unsigned strm_idx, strm_cnt;
            
            app_config.avi[i].dev_id = PJMEDIA_VID_INVALID_DEV;
            app_config.avi[i].slot = PJSUA_INVALID_ID;
            
            pjmedia_avi_dev_param_default(&avdp);
            avdp.path = app_config.avi[i].path;
            
            status =  pjmedia_avi_dev_alloc(avi_factory, &avdp, &avid);
            if (status != PJ_SUCCESS) {
                PJ_PERROR(1,(THIS_FILE, status,
                             "Error creating AVI player for %.*s",
                             (int)avdp.path.slen, avdp.path.ptr));
                goto on_error;
            }
            
            PJ_LOG(4,(THIS_FILE, "AVI player %.*s created, dev_id=%d",
                      (int)avdp.title.slen, avdp.title.ptr, avid));
            
            app_config.avi[i].dev_id = avid;
            if (app_config.avi_def_idx == PJSUA_INVALID_ID)
                app_config.avi_def_idx = i;
            
            strm_cnt = pjmedia_avi_streams_get_num_streams(avdp.avi_streams);
            for (strm_idx=0; strm_idx<strm_cnt; ++strm_idx) {
                pjmedia_port *aud;
                pjmedia_format *fmt;
                pjsua_conf_port_id slot;
                char fmt_name[5];
                
                aud = pjmedia_avi_streams_get_stream(avdp.avi_streams,
                                                     strm_idx);
                fmt = &aud->info.fmt;
                
                pjmedia_fourcc_name(fmt->id, fmt_name);
                
                if (fmt->id == PJMEDIA_FORMAT_PCM) {
                    status = pjsua_conf_add_port(app_config.pool, aud,
                                                 &slot);
                    if (status == PJ_SUCCESS) {
                        PJ_LOG(4,(THIS_FILE,
                                  "AVI %.*s: audio added to slot %d",
                                  (int)avdp.title.slen, avdp.title.ptr,
                                  slot));
                        app_config.avi[i].slot = slot;
                    }
                } else {
                    PJ_LOG(4,(THIS_FILE,
                              "AVI %.*s: audio ignored, format=%s",
                              (int)avdp.title.slen, avdp.title.ptr,
                              fmt_name));
                }
            }
        }
#else
        PJ_LOG(2,(THIS_FILE,
                  "Warning: --play-avi is ignored because AVI is disabled"));
#endif	/* PJMEDIA_VIDEO_DEV_HAS_AVI */
    }
    
    // Add UDP transport.
    {
        // Init transport config structure
        pjsua_transport_config cfg;
        pjsua_transport_config_default(&cfg);
        
        // Add UDP transport.
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, &transport_id);
        if (status != PJ_SUCCESS)
            goto on_error;
    }
    
    // Add TCP transport.
    {
        // Init transport config structure
        pjsua_transport_config cfg;
        pjsua_transport_config_default(&cfg);
        
        // Add TCP transport.
        status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, &transport_id);
        if (status != PJ_SUCCESS)
            goto on_error;
    }
    
    if (transport_id == -1) {
        PJ_LOG(1,(THIS_FILE, "Error: no transport is configured"));
        status = -1;
        goto on_error;
    }
    
    /* Optionally set codec orders */
    for (i=0; i<app_config.codec_cnt; ++i) {
        pjsua_codec_set_priority(&app_config.codec_arg[i],
                                 (pj_uint8_t)(PJMEDIA_CODEC_PRIO_NORMAL+i+9));

        pjsua_vid_codec_set_priority(&app_config.codec_arg[i],
                                     (pj_uint8_t)(PJMEDIA_CODEC_PRIO_NORMAL+i+9));
    }
    
    if (app_config.capture_dev  != PJSUA_INVALID_ID ||
        app_config.playback_dev != PJSUA_INVALID_ID) 
    {
        status = pjsua_set_snd_dev(app_config.capture_dev, 
                                   app_config.playback_dev);
        if (status != PJ_SUCCESS)
            goto on_error;
    }
    
    /* Init call setting */
    pjsua_call_setting_default(&call_opt);
    call_opt.aud_cnt = app_config.aud_cnt = 1;
    call_opt.vid_cnt = app_config.vid.vid_cnt = 1;
    
    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);
    
    pj_pool_release(tmp_pool);
    return status;
    
on_error:
    pj_pool_release(tmp_pool);
    return status;
}

- (void) makeCallTo:(NSString *)user {
    
    pj_status_t status;
    char callTo[100];
    
    sprintf(callTo, "sip:%s@%s:5060", [user UTF8String], "107.170.46.82");

    pj_str_t uri = pj_str(callTo);
    pjsua_call_setting settings;
    settings.aud_cnt = 1;
    settings.vid_cnt = 1;
    status = pjsua_call_make_call(_acc_id, &uri, 0, NULL, NULL, NULL);
}

- (void)answerCallFrom:(pjsua_call_info)callInfo withCallId:(pjsua_call_id)call_id {
    
    pjsua_call_setting setting;
    pjsua_call_setting_default(&setting);
    
    setting.vid_cnt = 1;
    setting.aud_cnt = 1;
    
    pjsua_call_answer2(call_id, &setting, 200, NULL, NULL);

}


/* The module instance. */
static pjsip_module mod_default_handler =
{
    NULL, NULL,				/* prev, next.		*/
    { "mod-default-handler", 19 },	/* Name.		*/
    -1,					/* Id			*/
    PJSIP_MOD_PRIORITY_APPLICATION+99,	/* Priority	        */
    NULL,				/* load()		*/
    NULL,				/* start()		*/
    NULL,				/* stop()		*/
    NULL,				/* unload()		*/
    &default_mod_on_rx_request,		/* on_rx_request()	*/
    NULL,				/* on_rx_response()	*/
    NULL,				/* on_tx_request.	*/
    NULL,				/* on_tx_response()	*/
    NULL,				/* on_tsx_state()	*/
    
};

/*****************************************************************************
 * A simple module to handle otherwise unhandled request. We will register
 * this with the lowest priority.
 */

/* Notification on incoming request */
static pj_bool_t default_mod_on_rx_request(pjsip_rx_data *rdata)
{
    pjsip_tx_data *tdata;
    pjsip_status_code status_code;
    pj_status_t status;
    
    /* Don't respond to ACK! */
    if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,
                         &pjsip_ack_method) == 0)
        return PJ_TRUE;
    
    /* Simple registrar */
    if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,
                         &pjsip_register_method) == 0)
    {
        simple_registrar(rdata);
        return PJ_TRUE;
    }
    
    /* Create basic response. */
    if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,
                         &pjsip_notify_method) == 0)
    {
        /* Unsolicited NOTIFY's, send with Bad Request */
        status_code = PJSIP_SC_BAD_REQUEST;
    } else {
        /* Probably unknown method */
        status_code = PJSIP_SC_METHOD_NOT_ALLOWED;
    }
    status = pjsip_endpt_create_response(pjsua_get_pjsip_endpt(),
                                         rdata, status_code,
                                         NULL, &tdata);
    if (status != PJ_SUCCESS) {
        pjsua_perror(THIS_FILE, "Unable to create response", status);
        return PJ_TRUE;
    }
    
    /* Add Allow if we're responding with 405 */
    if (status_code == PJSIP_SC_METHOD_NOT_ALLOWED) {
        const pjsip_hdr *cap_hdr;
        cap_hdr = pjsip_endpt_get_capability(pjsua_get_pjsip_endpt(),
                                             PJSIP_H_ALLOW, NULL);
        if (cap_hdr) {
            pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr *)pjsip_hdr_clone(
                                                                       tdata->pool, cap_hdr));
        }
    }
    
    /* Add User-Agent header */
    {
        pj_str_t user_agent;
        char tmp[80];
        const pj_str_t USER_AGENT = { "User-Agent", 10};
        pjsip_hdr *h;
        
        pj_ansi_snprintf(tmp, sizeof(tmp), "PJSUA v%s/%s",
                         pj_get_version(), PJ_OS_NAME);
        pj_strdup2_with_null(tdata->pool, &user_agent, tmp);
        
        h = (pjsip_hdr*) pjsip_generic_string_hdr_create(tdata->pool,
                                                         &USER_AGENT,
                                                         &user_agent);
        pjsip_msg_add_hdr(tdata->msg, h);
    }
    
    pjsip_endpt_send_response2(pjsua_get_pjsip_endpt(), rdata, tdata, 
                               NULL, NULL);
    
    return PJ_TRUE;
}

/*
 * A simple registrar, invoked by default_mod_on_rx_request()
 */
static void simple_registrar(pjsip_rx_data *rdata)
{
    pjsip_tx_data *tdata;
    const pjsip_expires_hdr *exp;
    const pjsip_hdr *h;
    unsigned cnt = 0;
    pjsip_generic_string_hdr *srv;
    pj_status_t status;
    
    status = pjsip_endpt_create_response(pjsua_get_pjsip_endpt(),
                                         rdata, 200, NULL, &tdata);
    if (status != PJ_SUCCESS)
        return;
    
    exp = (pjsip_expires_hdr *)pjsip_msg_find_hdr(rdata->msg_info.msg,
                                                  PJSIP_H_EXPIRES, NULL);
    
    h = rdata->msg_info.msg->hdr.next;
    while (h != &rdata->msg_info.msg->hdr) {
        if (h->type == PJSIP_H_CONTACT) {
            const pjsip_contact_hdr *c = (const pjsip_contact_hdr*)h;
            int e = c->expires;
            
            if (e < 0) {
                if (exp)
                    e = exp->ivalue;
                else
                    e = 3600;
            }
            
            if (e > 0) {
                pjsip_contact_hdr *nc = (pjsip_contact_hdr *)pjsip_hdr_clone(
                                                                             tdata->pool, h);
                nc->expires = e;
                pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)nc);
                ++cnt;
            }
        }
        h = h->next;
    }
    
    srv = pjsip_generic_string_hdr_create(tdata->pool, NULL, NULL);
    srv->name = pj_str("Server");
    srv->hvalue = pj_str("pjsua simple registrar");
    pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)srv);
    
    pjsip_endpt_send_response2(pjsua_get_pjsip_endpt(),
                               rdata, tdata, NULL, NULL);
}



/* Set default config. */
static void loadDefault_config()
{
    char tmp[80];
    unsigned i;
    pjsua_app_config *cfg = &app_config;
    
    
    
    pjsua_config_default(&cfg->cfg);
    pj_ansi_sprintf(tmp, "PJSUA v%s %s", pj_get_version(),
                    pj_get_sys_info()->info.ptr);
//    pj_strdup2_with_null(app_config.pool, &cfg->cfg.user_agent, tmp);
    
    pjsua_logging_config_default(&cfg->log_cfg);
    pjsua_media_config_default(&cfg->media_cfg);
    pjsua_transport_config_default(&cfg->udp_cfg);
    cfg->udp_cfg.port = 5060;
    pjsua_transport_config_default(&cfg->rtp_cfg);
    cfg->rtp_cfg.port = 4000;
    cfg->redir_op = PJSIP_REDIRECT_ACCEPT_REPLACE;
    cfg->duration = PJSUA_APP_NO_LIMIT_DURATION;
    cfg->wav_id = PJSUA_INVALID_ID;
    cfg->rec_id = PJSUA_INVALID_ID;
    cfg->wav_port = PJSUA_INVALID_ID;
    cfg->rec_port = PJSUA_INVALID_ID;
    cfg->mic_level = cfg->speaker_level = 1.0;
    cfg->capture_dev = PJSUA_INVALID_ID;
    cfg->playback_dev = PJSUA_INVALID_ID;
    cfg->capture_lat = PJMEDIA_SND_DEFAULT_REC_LATENCY;
    cfg->playback_lat = PJMEDIA_SND_DEFAULT_PLAY_LATENCY;
    cfg->ringback_slot = PJSUA_INVALID_ID;
    cfg->ring_slot = PJSUA_INVALID_ID;
    
    for (i=0; i<PJ_ARRAY_SIZE(cfg->acc_cfg); ++i)
        pjsua_acc_config_default(&cfg->acc_cfg[i]);
    
    for (i=0; i<PJ_ARRAY_SIZE(cfg->buddy_cfg); ++i)
        pjsua_buddy_config_default(&cfg->buddy_cfg[i]);
    
    cfg->vid.vcapture_dev = PJMEDIA_VID_DEFAULT_CAPTURE_DEV;
    cfg->vid.vrender_dev = PJMEDIA_VID_DEFAULT_RENDER_DEV;
    cfg->aud_cnt = 1;
    
    cfg->avi_def_idx = PJSUA_INVALID_ID;
    
    cfg->use_cli = PJ_FALSE;
    cfg->cli_cfg.cli_fe = CLI_FE_CONSOLE;
    cfg->cli_cfg.telnet_cfg.port = 0;
}

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}


/*
 * Handler when invite state has changed.
 */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info call_info;
    
    PJ_UNUSED_ARG(e);
    
    pjsua_call_get_info(call_id, &call_info);
    
    if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
        
//        /* Stop all ringback for this call */
//        ring_stop(call_id);
        
        /* Cancel duration timer, if any */
        if (app_config.call_data[call_id].timer.id != PJSUA_INVALID_ID) {
            app_call_data *cd = &app_config.call_data[call_id];
            pjsip_endpoint *endpt = pjsua_get_pjsip_endpt();
            
            cd->timer.id = PJSUA_INVALID_ID;
            pjsip_endpt_cancel_timer(endpt, &cd->timer);
        }
        
        /* Rewind play file when hangup automatically,
         * since file is not looped
         */
        if (app_config.auto_play_hangup)
            pjsua_player_set_pos(app_config.wav_id, 0);
        
        
        PJ_LOG(3,(THIS_FILE, "Call %d is DISCONNECTED [reason=%d (%s)]",
                  call_id,
                  call_info.last_status,
                  call_info.last_status_text.ptr));
        
        if (call_id == current_call) {
            find_next_call();
        }
        
        /* Dump media state upon disconnected */
        if (1) {
            PJ_LOG(5,(THIS_FILE,
                      "Call %d disconnected, dumping media stats..",
                      call_id));
            log_call_dump(call_id);
        }
        
    } else {
        
        if (app_config.duration != PJSUA_APP_NO_LIMIT_DURATION &&
            call_info.state == PJSIP_INV_STATE_CONFIRMED)
        {
            /* Schedule timer to hangup call after the specified duration */
            app_call_data *cd = &app_config.call_data[call_id];
            pjsip_endpoint *endpt = pjsua_get_pjsip_endpt();
            pj_time_val delay;
            
            cd->timer.id = call_id;
            delay.sec = app_config.duration;
            delay.msec = 0;
            pjsip_endpt_schedule_timer(endpt, &cd->timer, &delay);
        }
        
        if (call_info.state == PJSIP_INV_STATE_EARLY) {
            int code;
            pj_str_t reason;
            pjsip_msg *msg;
            
            /* This can only occur because of TX or RX message */
            pj_assert(e->type == PJSIP_EVENT_TSX_STATE);
            
            if (e->body.tsx_state.type == PJSIP_EVENT_RX_MSG) {
                msg = e->body.tsx_state.src.rdata->msg_info.msg;
            } else {
                msg = e->body.tsx_state.src.tdata->msg;
            }
            
            code = msg->line.status.code;
            reason = msg->line.status.reason;
            
//            /* Start ringback for 180 for UAC unless there's SDP in 180 */
//            if (call_info.role==PJSIP_ROLE_UAC && code==180 && 
//                msg->body == NULL && 
//                call_info.media_status==PJSUA_CALL_MEDIA_NONE) 
//            {
//                ringback_start(call_id);
//            }
            
            PJ_LOG(3,(THIS_FILE, "Call %d state changed to %s (%d %.*s)", 
                      call_id, call_info.state_text.ptr,
                      code, (int)reason.slen, reason.ptr));
        } else {
            PJ_LOG(3,(THIS_FILE, "Call %d state changed to %s", 
                      call_id,
                      call_info.state_text.ptr));
        }
        
        if (current_call==PJSUA_INVALID_ID)
            current_call = call_id;
        
    }
}

/*
 * Callback on media state changed event.
 * The action may connect the call to sound device, to file, or
 * to loop the call.
 */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info call_info;
    unsigned mi;
    pj_bool_t has_error = PJ_FALSE;
    
    pjsua_call_get_info(call_id, &call_info);
    
    for (mi=0; mi<call_info.media_cnt; ++mi) {
        on_call_generic_media_state(&call_info, mi, &has_error);
        
        switch (call_info.media[mi].type) {
            case PJMEDIA_TYPE_AUDIO:
                on_call_audio_state(&call_info, mi, &has_error);
                break;
            case PJMEDIA_TYPE_VIDEO:
                on_call_video_state(&call_info, mi, &has_error);
                break;
            default:
                /* Make gcc happy about enum not handled by switch/case */
                break;
        }
    }
    
    if (has_error) {
        pj_str_t reason = pj_str("Media failed");
        pjsua_call_hangup(call_id, 500, &reason, NULL);
    }
    
#if PJSUA_HAS_VIDEO
    /* Check if remote has just tried to enable video */
    if (call_info.rem_offerer && call_info.rem_vid_cnt)
    {
        int vid_idx;
        
        /* Check if there is active video */
        vid_idx = pjsua_call_get_vid_stream_idx(call_id);
        if (vid_idx == -1 || call_info.media[vid_idx].dir == PJMEDIA_DIR_NONE) {
            PJ_LOG(3,(THIS_FILE,
                      "Just rejected incoming video offer on call %d, "
                      "use \"vid call enable %d\" or \"vid call add\" to "
                      "enable video!", call_id, vid_idx));
        }
    }
#endif
}


/* General processing for media state. "mi" is the media index */
static void on_call_generic_media_state(pjsua_call_info *ci, unsigned mi,
                                        pj_bool_t *has_error)
{
    const char *status_name[] = {
        "None",
        "Active",
        "Local hold",
        "Remote hold",
        "Error"
    };
    
    PJ_UNUSED_ARG(has_error);
    
    pj_assert(ci->media[mi].status <= PJ_ARRAY_SIZE(status_name));
    pj_assert(PJSUA_CALL_MEDIA_ERROR == 4);
    
    PJ_LOG(4,(THIS_FILE, "Call %d media %d [type=%s], status is %s",
              ci->id, mi, pjmedia_type_name(ci->media[mi].type),
              status_name[ci->media[mi].status]));
}

/* Process audio media state. "mi" is the media index. */
static void on_call_audio_state(pjsua_call_info *ci, unsigned mi,
                                pj_bool_t *has_error)
{
    PJ_UNUSED_ARG(has_error);
    
//    /* Stop ringback */
//    ring_stop(ci->id);
    
    /* Connect ports appropriately when media status is ACTIVE or REMOTE HOLD,
     * otherwise we should NOT connect the ports.
     */
    if (ci->media[mi].status == PJSUA_CALL_MEDIA_ACTIVE ||
        ci->media[mi].status == PJSUA_CALL_MEDIA_REMOTE_HOLD)
    {
        pj_bool_t connect_sound = PJ_TRUE;
        pj_bool_t disconnect_mic = PJ_FALSE;
        pjsua_conf_port_id call_conf_slot;
        
        call_conf_slot = ci->media[mi].stream.aud.conf_slot;
        
        /* Loopback sound, if desired */
        if (app_config.auto_loop) {
            pjsua_conf_connect(call_conf_slot, call_conf_slot);
            connect_sound = PJ_FALSE;
        }
        
        /* Automatically record conversation, if desired */
        if (app_config.auto_rec && app_config.rec_port != PJSUA_INVALID_ID) {
            pjsua_conf_connect(call_conf_slot, app_config.rec_port);
        }
        
        /* Stream a file, if desired */
        if ((app_config.auto_play || app_config.auto_play_hangup) &&
            app_config.wav_port != PJSUA_INVALID_ID)
        {
            pjsua_conf_connect(app_config.wav_port, call_conf_slot);
            connect_sound = PJ_FALSE;
        }
        
        /* Stream AVI, if desired */
        if (app_config.avi_auto_play &&
            app_config.avi_def_idx != PJSUA_INVALID_ID &&
            app_config.avi[app_config.avi_def_idx].slot != PJSUA_INVALID_ID)
        {
            pjsua_conf_connect(app_config.avi[app_config.avi_def_idx].slot,
                               call_conf_slot);
            disconnect_mic = PJ_TRUE;
        }
        
        /* Put call in conference with other calls, if desired */
        if (app_config.auto_conf) {
            pjsua_call_id call_ids[PJSUA_MAX_CALLS];
            unsigned call_cnt=PJ_ARRAY_SIZE(call_ids);
            unsigned i;
            
            /* Get all calls, and establish media connection between
             * this call and other calls.
             */
            pjsua_enum_calls(call_ids, &call_cnt);
            
            for (i=0; i<call_cnt; ++i) {
                if (call_ids[i] == ci->id)
                    continue;
                
                if (!pjsua_call_has_media(call_ids[i]))
                    continue;
                
                pjsua_conf_connect(call_conf_slot,
                                   pjsua_call_get_conf_port(call_ids[i]));
                pjsua_conf_connect(pjsua_call_get_conf_port(call_ids[i]),
                                   call_conf_slot);
                
                /* Automatically record conversation, if desired */
                if (app_config.auto_rec && app_config.rec_port !=
                    PJSUA_INVALID_ID)
                {
                    pjsua_conf_connect(pjsua_call_get_conf_port(call_ids[i]),
                                       app_config.rec_port);
                }
                
            }
            
            /* Also connect call to local sound device */
            connect_sound = PJ_TRUE;
        }
        
        /* Otherwise connect to sound device */
        if (connect_sound) {
            pjsua_conf_connect(call_conf_slot, 0);
            if (!disconnect_mic)
                pjsua_conf_connect(0, call_conf_slot);
            
            /* Automatically record conversation, if desired */
            if (app_config.auto_rec && app_config.rec_port != PJSUA_INVALID_ID)
            {
                pjsua_conf_connect(call_conf_slot, app_config.rec_port);
                pjsua_conf_connect(0, app_config.rec_port);
            }
        }
    }
}

/* Process video media state. "mi" is the media index. */
static void on_call_video_state(pjsua_call_info *ci, unsigned mi,
                                pj_bool_t *has_error)
{
    if (ci->media_status != PJSUA_CALL_MEDIA_ACTIVE)
        return;
    
    arrange_window(ci->media[mi].stream.vid.win_in);
    
    PJ_UNUSED_ARG(has_error);
}

/**
 * Handler when there is incoming call.
 */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                             pjsip_rx_data *rdata)
{
    pjsua_call_info ci;
    
    pjsua_call_get_info(call_id, &ci);
    
    //    /* Automatically answer incoming calls with 200/OK */
//    pjsua_call_answer(call_id, 200, NULL, NULL);
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"incoming" object:nil];

    [[PJSIP sharedPJSIP] answerCallFrom:ci withCallId:call_id];
}

/*
 * Handler when a transaction within a call has changed state.
 */
static void on_call_tsx_state(pjsua_call_id call_id,
                              pjsip_transaction *tsx,
                              pjsip_event *e)
{
    const pjsip_method info_method =
    {
        PJSIP_OTHER_METHOD,
        { "INFO", 4 }
    };
    
    if (pjsip_method_cmp(&tsx->method, &info_method)==0) {
        /*
         * Handle INFO method.
         */
        const pj_str_t STR_APPLICATION = { "application", 11};
        const pj_str_t STR_DTMF_RELAY  = { "dtmf-relay", 10 };
        pjsip_msg_body *body = NULL;
        pj_bool_t dtmf_info = PJ_FALSE;
        
        if (tsx->role == PJSIP_ROLE_UAC) {
            if (e->body.tsx_state.type == PJSIP_EVENT_TX_MSG)
                body = e->body.tsx_state.src.tdata->msg->body;
            else
                body = e->body.tsx_state.tsx->last_tx->msg->body;
        } else {
            if (e->body.tsx_state.type == PJSIP_EVENT_RX_MSG)
                body = e->body.tsx_state.src.rdata->msg_info.msg->body;
        }
        
        /* Check DTMF content in the INFO message */
        if (body && body->len &&
            pj_stricmp(&body->content_type.type, &STR_APPLICATION)==0 &&
            pj_stricmp(&body->content_type.subtype, &STR_DTMF_RELAY)==0)
        {
            dtmf_info = PJ_TRUE;
        }
        
        if (dtmf_info && tsx->role == PJSIP_ROLE_UAC &&
            (tsx->state == PJSIP_TSX_STATE_COMPLETED ||
             (tsx->state == PJSIP_TSX_STATE_TERMINATED &&
              e->body.tsx_state.prev_state != PJSIP_TSX_STATE_COMPLETED)))
        {
            /* Status of outgoing INFO request */
            if (tsx->status_code >= 200 && tsx->status_code < 300) {
                PJ_LOG(4,(THIS_FILE,
                          "Call %d: DTMF sent successfully with INFO",
                          call_id));
            } else if (tsx->status_code >= 300) {
                PJ_LOG(4,(THIS_FILE,
                          "Call %d: Failed to send DTMF with INFO: %d/%.*s",
                          call_id,
                          tsx->status_code,
                          (int)tsx->status_text.slen,
                          tsx->status_text.ptr));
            }
        } else if (dtmf_info && tsx->role == PJSIP_ROLE_UAS &&
                   tsx->state == PJSIP_TSX_STATE_TRYING)
        {
            /* Answer incoming INFO with 200/OK */
            pjsip_rx_data *rdata;
            pjsip_tx_data *tdata;
            pj_status_t status;
            
            rdata = e->body.tsx_state.src.rdata;
            
            if (rdata->msg_info.msg->body) {
                status = pjsip_endpt_create_response(tsx->endpt, rdata,
                                                     200, NULL, &tdata);
                if (status == PJ_SUCCESS)
                    status = pjsip_tsx_send_msg(tsx, tdata);
                
                PJ_LOG(3,(THIS_FILE, "Call %d: incoming INFO:\n%.*s", 
                          call_id,
                          (int)rdata->msg_info.msg->body->len,
                          rdata->msg_info.msg->body->data));
            } else {
                status = pjsip_endpt_create_response(tsx->endpt, rdata,
                                                     400, NULL, &tdata);
                if (status == PJ_SUCCESS)
                    status = pjsip_tsx_send_msg(tsx, tdata);
            }
        }
    }
}

/*
 * DTMF callback.
 */
static void call_on_dtmf_callback(pjsua_call_id call_id, int dtmf)
{
    PJ_LOG(3,(THIS_FILE, "Incoming DTMF on call %d: %c", call_id, dtmf));
}

/*
 * Redirection handler.
 */
static pjsip_redirect_op call_on_redirected(pjsua_call_id call_id,
                                            const pjsip_uri *target,
                                            const pjsip_event *e)
{
    PJ_UNUSED_ARG(e);
    
    if (app_config.redir_op == PJSIP_REDIRECT_PENDING) {
        char uristr[PJSIP_MAX_URL_SIZE];
        int len;
        
        len = pjsip_uri_print(PJSIP_URI_IN_FROMTO_HDR, target, uristr,
                              sizeof(uristr));
        if (len < 1) {
            pj_ansi_strcpy(uristr, "--URI too long--");
        }
        
        PJ_LOG(3,(THIS_FILE, "Call %d is being redirected to %.*s. "
                  "Press 'Ra' to accept+replace To header, 'RA' to accept, "
                  "'Rr' to reject, or 'Rd' to disconnect.",
                  call_id, len, uristr));
    }
    
    return app_config.redir_op;
}


/*
 * Handler registration status has changed.
 */
static void on_reg_state(pjsua_acc_id acc_id)
{
    PJ_UNUSED_ARG(acc_id);
    
    // Log already written.
}

/*
 * Handler for incoming presence subscription request
 */
static void on_incoming_subscribe(pjsua_acc_id acc_id,
                                  pjsua_srv_pres *srv_pres,
                                  pjsua_buddy_id buddy_id,
                                  const pj_str_t *from,
                                  pjsip_rx_data *rdata,
                                  pjsip_status_code *code,
                                  pj_str_t *reason,
                                  pjsua_msg_data *msg_data)
{
    /* Just accept the request (the default behavior) */
    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(srv_pres);
    PJ_UNUSED_ARG(buddy_id);
    PJ_UNUSED_ARG(from);
    PJ_UNUSED_ARG(rdata);
    PJ_UNUSED_ARG(code);
    PJ_UNUSED_ARG(reason);
    PJ_UNUSED_ARG(msg_data);
}

/**
 * Call transfer request status.
 */
static void on_call_transfer_status(pjsua_call_id call_id,
                                    int status_code,
                                    const pj_str_t *status_text,
                                    pj_bool_t final,
                                    pj_bool_t *p_cont)
{
    PJ_LOG(3,(THIS_FILE, "Call %d: transfer status=%d (%.*s) %s",
              call_id, status_code,
              (int)status_text->slen, status_text->ptr,
              (final ? "[final]" : "")));
    
    if (status_code/100 == 2) {
        PJ_LOG(3,(THIS_FILE,
                  "Call %d: call transferred successfully, disconnecting call",
                  call_id));
        pjsua_call_hangup(call_id, PJSIP_SC_GONE, NULL, NULL);
        *p_cont = PJ_FALSE;
    }
}

/*
 * Notification that call is being replaced.
 */
static void on_call_replaced(pjsua_call_id old_call_id,
                             pjsua_call_id new_call_id)
{
    pjsua_call_info old_ci, new_ci;
    
    pjsua_call_get_info(old_call_id, &old_ci);
    pjsua_call_get_info(new_call_id, &new_ci);
    
    PJ_LOG(3,(THIS_FILE, "Call %d with %.*s is being replaced by "
              "call %d with %.*s",
              old_call_id,
              (int)old_ci.remote_info.slen, old_ci.remote_info.ptr,
              new_call_id,
              (int)new_ci.remote_info.slen, new_ci.remote_info.ptr));
}

/*
 * NAT type detection callback.
 */
static void on_nat_detect(const pj_stun_nat_detect_result *res)
{
    if (res->status != PJ_SUCCESS) {
        pjsua_perror(THIS_FILE, "NAT detection failed", res->status);
    } else {
        PJ_LOG(3, (THIS_FILE, "NAT detected as %s", res->nat_type_name));
    }
}

/*
 * MWI indication
 */
static void on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info)
{
    pj_str_t body;
    
    PJ_LOG(3,(THIS_FILE, "Received MWI for acc %d:", acc_id));
    
    if (mwi_info->rdata->msg_info.ctype) {
        const pjsip_ctype_hdr *ctype = mwi_info->rdata->msg_info.ctype;
        
        PJ_LOG(3,(THIS_FILE, " Content-Type: %.*s/%.*s",
                  (int)ctype->media.type.slen,
                  ctype->media.type.ptr,
                  (int)ctype->media.subtype.slen,
                  ctype->media.subtype.ptr));
    }
    
    if (!mwi_info->rdata->msg_info.msg->body) {
        PJ_LOG(3,(THIS_FILE, "  no message body"));
        return;
    }
    
    body.ptr = (char *)mwi_info->rdata->msg_info.msg->body->data;
    body.slen = mwi_info->rdata->msg_info.msg->body->len;
    
    PJ_LOG(3,(THIS_FILE, " Body:\n%.*s", (int)body.slen, body.ptr));
}


/*
 * Transport status notification
 */
static void on_transport_state(pjsip_transport *tp,
                               pjsip_transport_state state,
                               const pjsip_transport_state_info *info)
{
    char host_port[128];
    
    pj_ansi_snprintf(host_port, sizeof(host_port), "[%.*s:%d]",
                     (int)tp->remote_name.host.slen,
                     tp->remote_name.host.ptr,
                     tp->remote_name.port);
    
    switch (state) {
        case PJSIP_TP_STATE_CONNECTED:
        {
            PJ_LOG(3,(THIS_FILE, "SIP %s transport is connected to %s",
                      tp->type_name, host_port));
        }
            break;
            
        case PJSIP_TP_STATE_DISCONNECTED:
        {
            char buf[100];
            
            snprintf(buf, sizeof(buf), "SIP %s transport is disconnected "
                     "from %s", tp->type_name, host_port);
            pjsua_perror(THIS_FILE, buf, info->status);
        }
            break;
            
        default:
            break;
    }
    
#if defined(PJSIP_HAS_TLS_TRANSPORT) && PJSIP_HAS_TLS_TRANSPORT!=0
    
    if (!pj_ansi_stricmp(tp->type_name, "tls") && info->ext_info &&
        (state == PJSIP_TP_STATE_CONNECTED ||
         ((pjsip_tls_state_info*)info->ext_info)->
         ssl_sock_info->verify_status != PJ_SUCCESS))
    {
        pjsip_tls_state_info *tls_info = (pjsip_tls_state_info*)info->ext_info;
        pj_ssl_sock_info *ssl_sock_info = tls_info->ssl_sock_info;
        char buf[2048];
        const char *verif_msgs[32];
        unsigned verif_msg_cnt;
        
        /* Dump server TLS cipher */
        PJ_LOG(4,(THIS_FILE, "TLS cipher used: 0x%06X/%s",
                  ssl_sock_info->cipher,
                  pj_ssl_cipher_name(ssl_sock_info->cipher) ));
        
        /* Dump server TLS certificate */
        pj_ssl_cert_info_dump(ssl_sock_info->remote_cert_info, "  ",
                              buf, sizeof(buf));
        PJ_LOG(4,(THIS_FILE, "TLS cert info of %s:\n%s", host_port, buf));
        
        /* Dump server TLS certificate verification result */
        verif_msg_cnt = PJ_ARRAY_SIZE(verif_msgs);
        pj_ssl_cert_get_verify_status_strings(ssl_sock_info->verify_status,
                                              verif_msgs, &verif_msg_cnt);
        PJ_LOG(3,(THIS_FILE, "TLS cert verification result of %s : %s",
                  host_port,
                  (verif_msg_cnt == 1? verif_msgs[0]:"")));
        if (verif_msg_cnt > 1) {
            unsigned i;
            for (i = 0; i < verif_msg_cnt; ++i)
                PJ_LOG(3,(THIS_FILE, "- %s", verif_msgs[i]));
        }
        
        if (ssl_sock_info->verify_status &&
            !app_config.udp_cfg.tls_setting.verify_server) 
        {
            PJ_LOG(3,(THIS_FILE, "PJSUA is configured to ignore TLS cert "
                      "verification errors"));
        }
    }
    
#endif
}

/*
 * Notification on sound device operation.
 */
static pj_status_t on_snd_dev_operation(int operation)
{
    PJ_LOG(3,(THIS_FILE, "Turning sound device %s", (operation? "ON":"OFF")));
    return PJ_SUCCESS;
}

/* Callback on media events */
static void on_call_media_event(pjsua_call_id call_id,
                                unsigned med_idx,
                                pjmedia_event *event)
{
    char event_name[5];
    
    PJ_LOG(5,(THIS_FILE, "Event %s",
              pjmedia_fourcc_name(event->type, event_name)));
    
#if PJSUA_HAS_VIDEO
    if (event->type == PJMEDIA_EVENT_FMT_CHANGED) {
        /* Adjust renderer window size to original video size */
        pjsua_call_info ci;
        
        pjsua_call_get_info(call_id, &ci);
        
        if ((ci.media[med_idx].type == PJMEDIA_TYPE_VIDEO) &&
            (ci.media[med_idx].dir & PJMEDIA_DIR_DECODING))
        {
            pjsua_vid_win_id wid;
            pjmedia_rect_size size;
            pjsua_vid_win_info win_info;
            
            wid = ci.media[med_idx].stream.vid.win_in;
            pjsua_vid_win_get_info(wid, &win_info);
            
            size = event->data.fmt_changed.new_fmt.det.vid.size;
            if (size.w != win_info.size.w || size.h != win_info.size.h) {
                pjsua_vid_win_set_size(wid, &size);
                
                /* Re-arrange video windows */
                arrange_window(PJSUA_INVALID_ID);
            }
        }
    }
#else
    PJ_UNUSED_ARG(call_id);
    PJ_UNUSED_ARG(med_idx);
    PJ_UNUSED_ARG(event);
#endif
}

@end