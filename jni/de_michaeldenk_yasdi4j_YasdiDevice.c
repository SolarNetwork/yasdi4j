/*
 * yasdi4j -- Java Binding for YASDI
 * Copyright (c) 2008 Michael Denk <code@michaeldenk.de>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <jni.h>
#include <libyasdimaster.h>
#include "de_michaeldenk_yasdi4j_YasdiDevice.h"
#include "yasdi4j.h"

jobject constructChannel(JNIEnv *env, jclass chanCls, jmethodID cid,
		jobject device,	DWORD hChan)
{
	jstring name, unit;
	char buf[MAX_TEXT_SIZE];
	jboolean hasText;
	WORD mask;
	int idx;
#ifdef HAS_GET_CHANNEL_ACCESS_RIGHTS
	BYTE rights;
	jclass accRightsCls;
	jmethodID accCID;
#endif
	jobject accRights;
	jobject chan;

	GetChannelName(hChan, buf, sizeof(buf) - 1);
	name = (*env)->NewStringUTF(env, buf);
	if (name == NULL) {
		return NULL;
	}

	GetChannelUnit(hChan, buf, sizeof(buf) - 1);
	unit = (*env)->NewStringUTF(env, buf);
	if (unit == NULL) {
		return NULL;
	}

	if (GetChannelStatTextCnt(hChan) > 0) {
		hasText = JNI_TRUE;
	} else {
		hasText = JNI_FALSE;
	}

	GetChannelMask((DWORD) hChan, &mask, &idx);

#ifdef HAS_GET_CHANNEL_ACCESS_RIGHTS
	GetChannelAccessRights(hChan, &rights);
	accRightsCls = (*env)->FindClass(env,
			"de/michaeldenk/yasdi4j/YasdiChannelAccessRights");
	if (accRightsCls == NULL) {
		return NULL;
	}
	accCID = (*env)->GetMethodID(env, accRightsCls, "<init>", "(ZZ)V");
	if (accCID == NULL) {
		return NULL;
	}
	accRights = (*env)->NewObject(env, accRightsCls, accCID,
			(jboolean) (rights & CAR_READ),
			(jboolean) (rights & CAR_WRITE));
	if (accRights == NULL) {
		return NULL;
	}
#else
	accRights = NULL;
#endif

	chan = (*env)->NewObject(env, chanCls, cid, (jint) hChan, device, name,
			unit, hasText, (jint) mask, accRights);
	if (chan == NULL) {
		return NULL;
	}
	(*env)->DeleteLocalRef(env, name);
	(*env)->DeleteLocalRef(env, unit);
	return chan;
}

JNIEXPORT jobjectArray JNICALL Java_de_michaeldenk_yasdi4j_YasdiDevice_c_1getChannels
  (JNIEnv *env, jobject this, jint hDev, jint mask, jint idx)
{
	DWORD chanHandles[MAX_NR_CHANNELS];
	DWORD numChans;
	jclass chanCls;
	jobjectArray chanArr;
	jmethodID cid;
	DWORD i;

	numChans = GetChannelHandles((DWORD) hDev, chanHandles,
			sizeof(chanHandles), (WORD) mask, (BYTE) idx);

	/* initialize the channels array */
	chanCls = (*env)->FindClass(env, "de/michaeldenk/yasdi4j/YasdiChannel");
	if (chanCls == NULL) {
		return NULL;
	}
	chanArr = (*env)->NewObjectArray(env, numChans, chanCls, NULL);
	if (chanArr == NULL) {
		return NULL;
	}

	/* construct the channels */
	cid = (*env)->GetMethodID(env, chanCls, "<init>",
			"(ILde/michaeldenk/yasdi4j/YasdiDevice;Ljava/lang/String;Ljava/lang/String;ZILde/michaeldenk/yasdi4j/YasdiChannelAccessRights;)V");
	if (cid == NULL) {
		return NULL;
	}
	for (i = 0; i < numChans; ++i) {
		jobject chan;
		chan = constructChannel(env, chanCls, cid, this, chanHandles[i]);
		if (chan == NULL) {
			return NULL;
		}
		(*env)->SetObjectArrayElement(env, chanArr, i, chan);
		(*env)->DeleteLocalRef(env, chan);
	}

	return chanArr;
}

JNIEXPORT jobject JNICALL Java_de_michaeldenk_yasdi4j_YasdiDevice_c_1findChannel
  (JNIEnv *env, jobject this, jint hDev, jstring name)
{
	const char *chanName;
	DWORD hChan;
	jclass chanCls;
	jmethodID cid;

	/* search the channel */
	chanName = (*env)->GetStringUTFChars(env, name, NULL);
	hChan = FindChannelName((DWORD) hDev, chanName);
	(*env)->ReleaseStringUTFChars(env, name, chanName);

	if (hChan == 0) {
		return NULL;
	}

	chanCls = (*env)->FindClass(env, "de/michaeldenk/yasdi4j/YasdiChannel");
	if (chanCls == NULL) {
		return NULL;
	}

	/* construct the channels */
	cid = (*env)->GetMethodID(env, chanCls, "<init>",
			"(ILde/michaeldenk/yasdi4j/YasdiDevice;Ljava/lang/String;Ljava/lang/String;ZILde/michaeldenk/yasdi4j/YasdiChannelAccessRights;)V");
	if (cid == NULL) {
		return NULL;
	}

	return constructChannel(env, chanCls, cid, this, hChan);
}
