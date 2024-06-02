/*
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SARA4_PPP_CellularNetwork.h"
#include "rtos/ThisThread.h"

using namespace std::chrono_literals;
using namespace mbed;

SARA4_PPP_CellularNetwork::SARA4_PPP_CellularNetwork(ATHandler &atHandler, AT_CellularDevice &device) : AT_CellularNetwork(atHandler, device)
{
}

SARA4_PPP_CellularNetwork::~SARA4_PPP_CellularNetwork()
{
}

nsapi_error_t SARA4_PPP_CellularNetwork::set_operator_profile_impl(OperatorProfile opProf)
{
    tr_info("Setting MNO Profile.");
    nsapi_error_t ret = NSAPI_ERROR_OK;

    CellularNetwork::AttachStatus status;

    get_attach(status);

    if (status == Attached) {
        tr_debug("Operator profile can only be set in detached state");
        return NSAPI_ERROR_UNSUPPORTED;
    }

    _at.lock();
    switch(opProf) {
        case PROF_UNDEFINED:
            _at.at_cmd_discard("+UMNOPROF", "=0");
            break;
        case PROF_SIM_SELECT:
            _at.at_cmd_discard("+UMNOPROF", "=1");
            break;
        case PROF_ATT:
            _at.at_cmd_discard("+UMNOPROF", "=2");
            break;
        case PROF_VERIZON:
            _at.at_cmd_discard("+UMNOPROF", "=3");
            break;
        case PROF_TELSTRA:
            _at.at_cmd_discard("+UMNOPROF", "=4");
            break;
        case PROF_TMOBILE_US:
            _at.at_cmd_discard("+UMNOPROF", "=5");
            break;
        case PROF_CHINA_TELECOM:
            _at.at_cmd_discard("+UMNOPROF", "=6");
            break;
        case PROF_SPRINT:
            _at.at_cmd_discard("+UMNOPROF", "=8");
            break;
        case PROF_VODAFONE:
            _at.at_cmd_discard("+UMNOPROF", "=19");
            break;
        case PROF_NTTDOCOMO:
            _at.at_cmd_discard("+UMNOPROF", "=20");
            break;
        case PROF_TELUS:
             _at.at_cmd_discard("+UMNOPROF", "=21");
            break;
        case PROF_SOFTBANK:
             _at.at_cmd_discard("+UMNOPROF", "=28");
            break;
        case PROF_DEUTSCHE_TELEKOM:
             _at.at_cmd_discard("+UMNOPROF", "=31");
            break;
        case PROF_US_CELLULAR:
             _at.at_cmd_discard("+UMNOPROF", "=32");
            break;
        case PROF_VIVO:
            _at.at_cmd_discard("+UMNOPROF", "=33");
            break;
        case PROF_SKT:
            _at.at_cmd_discard("+UMNOPROF", "=39");
            break;
        case PROF_KDDI:
            _at.at_cmd_discard("+UMNOPROF", "=41");
            break;
        case PROF_ROGERS:
            _at.at_cmd_discard("+UMNOPROF", "=43");
            break;
        case PROF_CLARO_BRASIL:
            _at.at_cmd_discard("+UMNOPROF", "=44");
            break;
        case PROF_TIM_BRASIL:
            _at.at_cmd_discard("+UMNOPROF", "=45");
            break;
        case PROF_ORANGE_FRANCE:
            _at.at_cmd_discard("+UMNOPROF", "=46");
            break;
        case PROF_GLOBAL:
            _at.at_cmd_discard("+UMNOPROF", "=90");
            break;
        case PROF_STD_EUROPE:
            _at.at_cmd_discard("+UMNOPROF", "=100");
            break;
        default:
            _op_prof = PROF_UNKNOWN;
            ret = NSAPI_ERROR_UNSUPPORTED;
            break;
    }
    _at.unlock();

    if (ret == NSAPI_ERROR_OK) {
        ret = dgfly_reboot();
    }

    return ret;
}

nsapi_error_t SARA4_PPP_CellularNetwork::set_access_technology_impl(RadioAccessTechnology opRat)
{
    nsapi_error_t ret = NSAPI_ERROR_OK;

    CellularNetwork::AttachStatus status;

    get_attach(status);

    if (status == Attached) {
        tr_debug("RAT can only be set in detached state");
        return NSAPI_ERROR_UNSUPPORTED;
    }

    _at.lock();
    switch(opRat) {
        case RAT_EGPRS:
            _at.at_cmd_discard("+URAT", "=", "%d%d", 9, 8);
            break;
        case RAT_CATM1:
            _at.at_cmd_discard("+URAT", "=", "%d%d", 7, 8);
            break;
        case RAT_NB1:
            _at.at_cmd_discard("+URAT", "=", "%d%d", 8, 7);
            break;
        default:
            _op_act = RAT_UNKNOWN;
            ret = NSAPI_ERROR_UNSUPPORTED;
            break;
    }
    _at.unlock();
    if (ret == NSAPI_ERROR_OK) {
        ret = dgfly_reboot();
    }

    return ret;
}

nsapi_error_t SARA4_PPP_CellularNetwork::dgfly_reboot()
{
    _at.lock();
    _at.at_cmd_discard("+CFUN", "=15");

    nsapi_error_t err = NSAPI_ERROR_OK;
    Timer t1;
    t1.start();

    while (!(t1.elapsed_time() >= 30s)) {
        err = _at.at_cmd_discard("E0", "");
        if (err == NSAPI_ERROR_OK) {
            break;
        } else {
            _at.clear_error();
            rtos::ThisThread::sleep_for(1s);
        }
    }

    t1.stop();
    _at.unlock();
    return err;
}
