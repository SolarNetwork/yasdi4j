#ifndef YASDI4J_H
#define YASDI4J_H 1

/* guess YASDI version */
#define _YASDI_1_7_2	10000*1 + 7*1000 + 2
#define _YASDI_1_8		10000*1 + 8*1000
#ifdef API_EVENT_DEVICE_ADDED
#define _YASDI_VER _YASDI_1_7_2
#elif defined YASDI_EVENT_DEVICE_ADDED
#define _YASDI_VER _YASDI_1_8
#endif
#define _YASDI_VER _YASDI_1_8

/* the maximum nr of devices that can be handled */
#define MAX_NR_DEVICES 100

/* the maximum nr of channels (per device) that can be handled */
#define MAX_NR_CHANNELS 100

/* the maximum size of channel names, channel units, text channel values for
 * channels with text values, device names, device types, and driver names */
#define MAX_TEXT_SIZE 50

/* disabled in YASDI 1.7.2; function GetChannelAccessRights is
 * not exported (at least in the windows version) */
#if _YASDI_VER >= _YASDI_1_8
#define HAS_GET_CHANNEL_ACCESS_RIGHTS
#endif

#endif /* YASDI4J_H */
