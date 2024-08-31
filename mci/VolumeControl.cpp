// VolumeControl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


//#include <jni.h> 

#include "src_media_VolumeControl.h"  
#include "MyVolumeCtrl.h"

JNIEXPORT void JNICALL Java_src_media_VolumeControl_SetVolumeValue
(JNIEnv *env, jobject obj, jlong val){
    CMyVolumeCtrl s ;
	s.SetVolume(val);
}

JNIEXPORT jlong JNICALL Java_src_media_VolumeControl_GetVolumeValue
(JNIEnv *env, jobject obj){
    CMyVolumeCtrl g ;
	return g.GetVolume();
	
}