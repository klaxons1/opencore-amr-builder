#include <jni.h>
#include <stdint.h> // For intptr_t
#include "interf_dec.h" // AMR decoder header

JNIEXPORT jlong JNICALL Java_emulator_media_amr_AMRDecoderJni_initDecoder(JNIEnv* env, jclass clazz) {
    void* decoder = Decoder_Interface_init();
    return (jlong)(intptr_t)decoder;
}

JNIEXPORT jint JNICALL Java_emulator_media_amr_AMRDecoderJni_decodeFrame(
    JNIEnv* env, jclass clazz, jlong state, jbyteArray amrFrame, jshortArray pcmOut) {

    if (state == 0) return -1;

    jsize frameLen = (*env)->GetArrayLength(env, amrFrame);
    jsize pcmLen = (*env)->GetArrayLength(env, pcmOut);

    if (frameLen <= 0 || pcmLen < 160) return -1; // 160 shorts = 20ms frame

    jbyte* frame = (*env)->GetByteArrayElements(env, amrFrame, NULL);
    jshort* out = (*env)->GetShortArrayElements(env, pcmOut, NULL);
    if (!frame || !out) {
        if (frame) (*env)->ReleaseByteArrayElements(env, amrFrame, frame, JNI_ABORT);
        if (out) (*env)->ReleaseShortArrayElements(env, pcmOut, out, JNI_ABORT);
        return -1;
    }

    const unsigned char* frameData = (const unsigned char*)frame;
    void* decoder = (void*)(intptr_t)state;
    int len = Decoder_Interface_Decode(decoder, frameData, out, 0);

    (*env)->ReleaseByteArrayElements(env, amrFrame, frame, JNI_ABORT);
    (*env)->ReleaseShortArrayElements(env, pcmOut, out, 0);
    return len;
}

JNIEXPORT void JNICALL Java_emulator_media_amr_AMRDecoderJni_exitDecoder(JNIEnv* env, jclass clazz, jlong state) {
    if (state != 0) {
        Decoder_Interface_exit((void*)(intptr_t)state);
    }
}