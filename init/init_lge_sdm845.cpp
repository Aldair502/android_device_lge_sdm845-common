/*
   Copyright (C) 2007, The Android Open Source Project
   Copyright (c) 2016, The CyanogenMod Project
   Copyright (c) 2017, The LineageOS Project

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

using android::base::Trim;
using android::base::GetProperty;
using android::init::property_set;

void property_override(const std::string& name, const std::string& value)
{
    size_t valuelen = value.size();

    prop_info* pi = (prop_info*) __system_property_find(name.c_str());
    if (pi != nullptr) {
        __system_property_update(pi, value.c_str(), valuelen);
    }
    else {
        int rc = __system_property_add(name.c_str(), name.size(), value.c_str(), valuelen);
        if (rc < 0) {
            LOG(ERROR) << "property_set(\"" << name << "\", \"" << value << "\") failed: "
                       << "__system_property_add failed";
        }
    }
}

void init_target_properties()
{
    std::string device;
    std::string cmdline;
    bool unknownDevice = true;
    bool dualSim = false;
    bool isGlobal = false;

    android::base::ReadFileToString("/proc/cmdline", &cmdline);

    for (const auto& entry : android::base::Split(android::base::Trim(cmdline), " ")) {
        std::vector<std::string> pieces = android::base::Split(entry, "=");
        if (pieces.size() == 2) {
            if(pieces[0].compare("androidboot.vendor.lge.model.name") == 0)
            {
            	device = pieces[1];
		unknownDevice = false;
            } else if(pieces[0].compare("lge.dsds") == 0 && pieces[1].compare("dsds") == 0)
            {
		dualSim = true;
            } else if(pieces[0].compare("lge.ntcode_op") == 0 && pieces[1].compare("GLOBAL") == 0)
            {
                isGlobal = true;
            }
        }
    }

    if(unknownDevice)
    {
        device = "UNKNOWN";
    }

    if(dualSim)
    {
        property_set("persist.radio.multisim.config", "dsds");
    }

    if(isGlobal)
    {
        property_set("ro.product.name", "judypn_lao_eea");
        property_set("ro.product.vendor.name", "judypn_lao_eea");
    } else 
    {
        property_set("ro.product.name", "judypn_lao_com");
        property_set("ro.product.vendor.name", "judypn_lao_com");
    }

    property_set("ro.product.model", device);
    property_set("ro.vendor.product.model", device);
    property_set("ro.product.system.model", device);
}

void vendor_load_properties() {
    LOG(INFO) << "Loading vendor specific properties";
    init_target_properties();
}