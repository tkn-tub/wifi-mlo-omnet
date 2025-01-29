/*
 * MLOInetExtensions.h
 *
 *  Created on: 08.08.2024
 *      Author: ergenc
 */

#ifndef COMMON_MLOINETEXTENSIONS_H_
#define COMMON_MLOINETEXTENSIONS_H_

// include potential extensions of existing INET files

#include "inet/common/Protocol.h"

namespace inet {

const Protocol ieee80211be("ieee80211be", "Wi-Fi 7", Protocol::LinkLayer);

}

#endif /* COMMON_MLOINETEXTENSIONS_H_ */
