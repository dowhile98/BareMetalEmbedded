/**
 * \file            lwbtn_opts.h
 * \brief           LwBTN configuration file (TCS CICX1 Project)
 *
 * ACTION-012: Configured for keep-alive and click counting features.
 */

/*
 * Copyright (c) 2024 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwBTN - Lightweight button manager.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.0
 */
#ifndef LWBTN_OPTS_HDR_H
#define LWBTN_OPTS_HDR_H

/* ===== TCS CICX1 Configuration (ACTION-012) ===== */

/* Keep-alive settings (auto-repeat) */
#define LWBTN_CFG_USE_KEEPALIVE 1
#define LWBTN_CFG_TIME_KEEPALIVE_PERIOD 100 /* 100ms auto-repeat period */

/* Click detection settings */
#define LWBTN_CFG_USE_CLICK 1
#define LWBTN_CFG_TIME_CLICK_MIN 20                        /* Min 20ms press for valid click */
#define LWBTN_CFG_TIME_CLICK_MAX 300                       /* Max 300ms press for click (>300ms = long press) */
#define LWBTN_CFG_TIME_CLICK_MULTI_MAX 400                 /* Max 400ms between consecutive clicks */
#define LWBTN_CFG_CLICK_MAX_CONSECUTIVE 3                  /* Max 3 clicks detected (single/double/triple) */
#define LWBTN_CFG_CLICK_MAX_CONSECUTIVE_SEND_IMMEDIATELY 1 /* Send immediately when max reached */

/* Debounce settings */
#define LWBTN_CFG_TIME_DEBOUNCE_PRESS 20  /* 20ms debounce on press */
#define LWBTN_CFG_TIME_DEBOUNCE_RELEASE 0 /* No release debounce */

/* Dynamic timing (disabled for static footprint and predictability) */
#define LWBTN_CFG_TIME_DEBOUNCE_PRESS_DYNAMIC 0
#define LWBTN_CFG_TIME_DEBOUNCE_RELEASE_DYNAMIC 0
#define LWBTN_CFG_TIME_KEEPALIVE_PERIOD_DYNAMIC 0
#define LWBTN_CFG_TIME_CLICK_MIN_DYNAMIC 0
#define LWBTN_CFG_TIME_CLICK_MAX_DYNAMIC 0
#define LWBTN_CFG_TIME_CLICK_MULTI_MAX_DYNAMIC 0
#define LWBTN_CFG_CLICK_MAX_CONSECUTIVE_DYNAMIC 0

#endif /* LWBTN_OPTS_HDR_H */
