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
#include <string.h>
#include "de_michaeldenk_yasdi4j_YasdiChannel.h"
#include "yasdi4j.h"

JNIEXPORT void JNICALL Java_de_michaeldenk_yasdi4j_YasdiChannel_c_1updateValue
  (JNIEnv *env, jobject this, jint hChan, jint hDev, jint maxValAge)
{
	DWORD chanHandle;
	DWORD devHandle;
	double value;
	char valuetext[MAX_TEXT_SIZE];
	int res;
	jclass cls;
	jfieldID fid;
	jstring jstr;

	chanHandle = (DWORD) hChan;
	devHandle = (DWORD) hDev;

	/* get current data */
	res = GetChannelValue(chanHandle, devHandle, &value, valuetext,
			sizeof(valuetext) - 1, maxValAge);

	if (res != 0) {
		value = 0;
		strcpy(valuetext, "");
	}

	/* update Channel object */
	cls = (*env)->GetObjectClass(env, this);
	fid = (*env)->GetFieldID(env, cls, "value", "D");
	if (fid == NULL) {
		return;
	}
	(*env)->SetDoubleField(env, this, fid, (jdouble) value);
	jstr = (*env)->NewStringUTF(env, valuetext);
	if (jstr == NULL) {
		return;
	}
	fid = (*env)->GetFieldID(env, cls, "valueText", "Ljava/lang/String;");
	if (fid == NULL) {
		return;
	}
	(*env)->SetObjectField(env, this, fid, jstr);

	if (res != 0) {
		jclass exCls;
		char buf[50];

		if (res == -2) {
			snprintf(buf, sizeof(buf), "YASDI is in 'ShutDown' mode");
		} else if (res == -3) {
			snprintf(buf, sizeof(buf), "timeout getting value");
		} else if (res == -4) {
			snprintf(buf, sizeof(buf), "unknown error");
		} else if (res == -5) {
			snprintf(buf, sizeof(buf), "access denied");
		} else {
			snprintf(buf, sizeof(buf), "GetChannelValue returned %d", res);
		}

		exCls = (*env)->FindClass(env, "java/io/IOException");
		if (exCls != NULL) {
			(*env)->ThrowNew(env, exCls, buf);
		}
	}
}
