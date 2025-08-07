#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include "interf_dec.h"
#include <string.h> // Для memset

extern "C" {

JNIEXPORT jlong JNICALL Java_emulator_media_amr_AMRDecoderJni_initDecoder(JNIEnv* env, jclass) {
    void* decoder = Decoder_Interface_init();
    return reinterpret_cast<jlong>(decoder);
}

JNIEXPORT void JNICALL Java_emulator_media_amr_AMRDecoderJni_decodeFrame(
    JNIEnv* env, jobject obj, jlong state, jbyteArray amrFrame, jshortArray pcmOut) {
    
    // 1. Проверка валидности state
    if (state == 0) {
        // Логирование ошибки было бы полезно
        return;
    }

    // 2. Проверка размеров массивов
    jsize frameLen = env->GetArrayLength(amrFrame);
    jsize pcmLen = env->GetArrayLength(pcmOut);
    
    if (frameLen < 1 || pcmLen < 160) {
        return;
    }

    // 3. Безопасное получение указателей
    jbyte* frame = env->GetByteArrayElements(amrFrame, NULL);
    jshort* out = env->GetShortArrayElements(pcmOut, NULL);
    
    if (!frame || !out) {
        // Освобождаем ресурсы если что-то получили
        if (frame) env->ReleaseByteArrayElements(amrFrame, frame, JNI_ABORT);
        if (out) env->ReleaseShortArrayElements(pcmOut, out, JNI_ABORT);
        return;
    }

    // 4. Декодирование с проверкой
    const unsigned char* frameData = reinterpret_cast<const unsigned char*>(frame);
    void* decoder = reinterpret_cast<void*>(state);
    
    // Важно: AMR всегда декодируется в 160 сэмплов (320 байт)
    Decoder_Interface_Decode(decoder, frameData, out, 0);

    // 5. Безопасное освобождение ресурсов
    env->ReleaseByteArrayElements(amrFrame, frame, JNI_ABORT);
    env->ReleaseShortArrayElements(pcmOut, out, 0);
}

JNIEXPORT void JNICALL Java_emulator_media_amr_AMRDecoderJni_closeDecoder(
    JNIEnv* env, jobject obj, jlong state) {
    if (state != 0) {
        Decoder_Interface_exit(reinterpret_cast<void*>(state));
    }
}



#ifdef __cplusplus
}
#endif