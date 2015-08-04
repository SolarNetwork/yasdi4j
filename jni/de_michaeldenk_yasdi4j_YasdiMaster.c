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

#include <jni.h>
#include <libyasdimaster.h>
#include <stdio.h>
#include "de_michaeldenk_yasdi4j_YasdiMaster.h"
#include "yasdi4j.h"

JavaVM *g_jvm = NULL;
jobject g_this = NULL;

#if _YASDI_VER >= _YASDI_1_8
void eventCallback(BYTE event, DWORD param1)
{
#else
void eventCallback(BYTE event, DWORD param1, DWORD param2)
{
	UNUSED_VAR(param2);
#endif
	JNIEnv *env;
	DWORD sn;
	jclass cls;
	jmethodID mid;
	jstring jName, jType;
	char buf[MAX_TEXT_SIZE] = { 0 };

	(*g_jvm)->AttachCurrentThread(g_jvm, (void **) (void *) &env, NULL);
	if (env == NULL || g_this == NULL) {
		goto detach;
	}

	/* get device name */
	GetDeviceName(param1, buf, sizeof(buf) - 1);
	jName = (*env)->NewStringUTF(env, buf);
	if (jName == NULL) {
		goto detach;
	}

	/* get serial number */
	GetDeviceSN(param1, &sn);

	/* get device type */
	GetDeviceType(param1, buf, sizeof(buf) - 1);
	jType = (*env)->NewStringUTF(env, buf);
	if (jType == NULL) {
		goto detach;
	}

	cls = (*env)->GetObjectClass(env, g_this);
	switch (event) {
#if _YASDI_VER >= _YASDI_1_8
		case YASDI_EVENT_DEVICE_ADDED:
#else
		case API_EVENT_DEVICE_ADDED:
#endif
			mid = (*env)->GetMethodID(env, cls, "deviceAdded",
					"(ILjava/lang/String;JLjava/lang/String;)V");
			if (mid == NULL) {
				goto detach;
			}
			(*env)->CallVoidMethod(env, g_this, mid, (jint) param1, jName,
					(jlong) sn, jType);
			break;
#if _YASDI_VER >= _YASDI_1_8
		case YASDI_EVENT_DEVICE_REMOVED:
#else
		case API_EVENT_DEVICE_REMOVED:
#endif
			mid = (*env)->GetMethodID(env, cls, "deviceRemoved",
					"(ILjava/lang/String;JLjava/lang/String;)V");
			if (mid == NULL) {
				goto detach;
			}
			(*env)->CallVoidMethod(env, g_this, mid, (jint) param1, jName,
					(jlong) sn, jType);
			break;
	}

detach:
	(*g_jvm)->DetachCurrentThread(g_jvm);
}

JNIEXPORT void JNICALL Java_de_michaeldenk_yasdi4j_YasdiMaster_c_1initialize
  (JNIEnv *env, jobject this, jstring configFile)
{
	jclass cls;
	jfieldID fid;
	jobjectArray drivers;
	jclass drvCls;
	const char *cIniFileName;
	DWORD pDriverCount;
	jmethodID cid;
	unsigned int i;
	char buf[MAX_TEXT_SIZE];
	jstring jstr;
	int ret;

	/* call yasdiMasterInitialize */
	if (configFile == NULL) {
		cIniFileName = "yasdi.ini";
	} else {
		cIniFileName = (*env)->GetStringUTFChars(env, configFile, NULL);
		if (cIniFileName == NULL) {
			return;
		}
	}
	ret = yasdiMasterInitialize(cIniFileName, &pDriverCount);

	if (ret != 0) {
		char buf2[256];
		jclass exCls;

		if (ret == -1) {
			snprintf(buf2, sizeof(buf2),
					"INI file not found or not readable: %s", cIniFileName);
			(*env)->ReleaseStringUTFChars(env, configFile, cIniFileName);
		} else {
			snprintf(buf2, sizeof(buf2), "yasdiMasterInitialize returned %d",
					ret);
		}

		exCls = (*env)->FindClass(env, "java/io/IOException");
		if (exCls != NULL) {
			(*env)->ThrowNew(env, exCls, buf2);
		}
		return;
	}

	(*env)->ReleaseStringUTFChars(env, configFile, cIniFileName);

	/* collect drivers */
	cls = (*env)->GetObjectClass(env, this);
	fid = (*env)->GetFieldID(env, cls, "drivers",
			"[Lde/michaeldenk/yasdi4j/YasdiDriver;");
	if (fid == NULL) {
		return;
	}
	drvCls = (*env)->FindClass(env, "de/michaeldenk/yasdi4j/YasdiDriver");
	if (drvCls == NULL) {
		return;
	}
	drivers = (*env)->NewObjectArray(env, pDriverCount, drvCls, NULL);
	if (drivers == NULL) {
		return;
	}
	(*env)->SetObjectField(env, this, fid, drivers);

	cid = (*env)->GetMethodID(env, drvCls, "<init>", "(ILjava/lang/String;)V");
	if (cid == NULL) {
		return;
	}
	for (i = 0; i < pDriverCount; ++i) {
		yasdiMasterGetDriverName(i, buf, sizeof(buf) - 1);
		jstr = (*env)->NewStringUTF(env, buf);
		if (jstr == NULL) {
			return;
		}
		jobject drv = (*env)->NewObject(env, drvCls, cid, (jint) i, jstr);
		(*env)->DeleteLocalRef(env, jstr);
		(*env)->SetObjectArrayElement(env, drivers, i, drv);
		(*env)->DeleteLocalRef(env, drv);
	}

	/* register callback function */
	if (g_this == NULL) {
		(*env)->GetJavaVM(env, &g_jvm);
		g_this = (*env)->NewGlobalRef(env, this);
#if _YASDI_VER >= _YASDI_1_8
		yasdiMasterAddEventListener(eventCallback, YASDI_EVENT_DEVICE_DETECTION);
#else
		yasdiMasterAddEventListener(eventCallback);
#endif
	}
}

JNIEXPORT void JNICALL Java_de_michaeldenk_yasdi4j_YasdiMaster_c_1shutdown
  (JNIEnv *env, jobject this)
{
	UNUSED_VAR(this);
#if _YASDI_VER >= _YASDI_1_8
	yasdiMasterRemEventListener(eventCallback, YASDI_EVENT_DEVICE_DETECTION);
#else
	yasdiMasterRemEventListener(eventCallback);
#endif
	(*env)->DeleteGlobalRef(env, g_this);
	g_this = NULL;
	yasdiMasterShutdown();
}

JNIEXPORT void JNICALL Java_de_michaeldenk_yasdi4j_YasdiMaster_c_1reset
  (JNIEnv *env, jobject this)
{
	UNUSED_VAR(env);
	UNUSED_VAR(this);
	yasdiReset();
}

JNIEXPORT jboolean JNICALL Java_de_michaeldenk_yasdi4j_YasdiMaster_c_1setDriverOnline
  (JNIEnv *env, jobject this, jint DriverID)
{
	UNUSED_VAR(env);
	UNUSED_VAR(this);
	return (jboolean) yasdiMasterSetDriverOnline((DWORD) DriverID);
}

JNIEXPORT void JNICALL Java_de_michaeldenk_yasdi4j_YasdiMaster_c_1setDriverOffline
  (JNIEnv *env, jobject this, jint DriverID)
{
	UNUSED_VAR(env);
	UNUSED_VAR(this);
	yasdiMasterSetDriverOffline((DWORD) DriverID);
}

JNIEXPORT void JNICALL Java_de_michaeldenk_yasdi4j_YasdiMaster_c_1detectDevices
  (JNIEnv *env, jobject this, jint nrDevs)
{
	int ret;
	jclass cls;
	jfieldID fid;
	jobjectArray devArr;
	jclass devCls;
	jmethodID cid;
	unsigned int i;
	DWORD handles[MAX_NR_DEVICES];
	DWORD numDevs;

	/* detect >= nrDevs devices */
	ret = yasdiDoMasterCmdEx("detection", (DWORD) nrDevs, 0 /* unused */,
			0 /* unused */);

	/* try to go on even when we throw an exception because ret == -1
	 * may mean that some devices were found nevertheless
	 */
	numDevs = GetDeviceHandles(handles, sizeof(handles));

	/* initialize the devices array */
	cls = (*env)->GetObjectClass(env, this);
	fid = (*env)->GetFieldID(env, cls, "devices",
			"[Lde/michaeldenk/yasdi4j/YasdiDevice;");
	if (fid == NULL) {
		return;
	}
	devCls = (*env)->FindClass(env, "de/michaeldenk/yasdi4j/YasdiDevice");
	if (devCls == NULL) {
		return;
	}
	devArr = (*env)->NewObjectArray(env, numDevs, devCls, NULL);
	if (devArr == NULL) {
		return;
	}
	(*env)->SetObjectField(env, this, fid, devArr);

	/* construct the devices */
	cid = (*env)->GetMethodID(env, devCls, "<init>",
			"(Lde/michaeldenk/yasdi4j/YasdiMaster;ILjava/lang/String;JLjava/lang/String;)V");
	if (cid == NULL) {
		return;
	}
	for (i = 0; i < numDevs; ++i) {
		jstring name, type;
		DWORD sn;
		char buf[MAX_TEXT_SIZE];

		GetDeviceName(handles[i], buf, sizeof(buf) - 1);
		name = (*env)->NewStringUTF(env, buf);
		if (name == NULL) {
			return;
		}
		GetDeviceType(handles[i], buf, sizeof(buf) - 1);
		type = (*env)->NewStringUTF(env, buf);
		if (type == NULL) {
			return;
		}
		GetDeviceSN(handles[i], &sn);
		jobject dev = (*env)->NewObject(env, devCls, cid, this,
				(jint) handles[i], name, (jlong) sn, type);
		(*env)->DeleteLocalRef(env, name);
		(*env)->DeleteLocalRef(env, type);
		(*env)->SetObjectArrayElement(env, devArr, i, dev);
		(*env)->DeleteLocalRef(env, dev);
	}

	if (ret != 0) {
		char buf[100];
		jclass exCls;

		if (ret == -1) {
			snprintf(buf, sizeof(buf),
					"%li out of %li device(s) didn't answer; device detection failed",
					(DWORD) nrDevs - numDevs, (DWORD) nrDevs);
		} else if (ret == -2) {
			snprintf(buf, sizeof(buf), "Master is 'sleeping'");
		} else {
			snprintf(buf, sizeof(buf), "yasdiDoMasterCmdEx returned %d", ret);
		}

		exCls = (*env)->FindClass(env, "java/io/IOException");
		if (exCls != NULL) {
			(*env)->ThrowNew(env, exCls, buf);
		}
	}
}

JNIEXPORT jboolean JNICALL Java_de_michaeldenk_yasdi4j_YasdiMaster_c_1setAccessLevel
  (JNIEnv *env, jobject this, jstring jUser, jstring jPassword)
{
	UNUSED_VAR(this);
	const char *user;
	const char *password;
	jboolean result;

	user = (*env)->GetStringUTFChars(env, jUser, NULL);
	if (user == NULL) {
		return JNI_FALSE;
	}
	password = (*env)->GetStringUTFChars(env, jPassword, NULL);
	if (password == NULL) {
		(*env)->ReleaseStringUTFChars(env, jUser, user);
		return JNI_FALSE;
	}
	result = (jboolean) yasdiMasterSetAccessLevel(user, password);
	(*env)->ReleaseStringUTFChars(env, jUser, user);
	(*env)->ReleaseStringUTFChars(env, jPassword, password);
	return result;
}
