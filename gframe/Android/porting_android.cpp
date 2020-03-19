/*
Minetest
Copyright (C) 2014 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __ANDROID__
#error This file may only be compiled for android!
#endif

#include "porting_android.h"

#include <irrlicht.h>
#include <sstream>
#include <exception>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <android/log.h>
#include <vector>
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, "Edopro", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Edopro", __VA_ARGS__);
#include "../bufferio.h"
#include <thread>
#include "../sound_manager.h"

extern int main(int argc, char *argv[]);

void android_main(android_app *app)
{
	int retval = 0;
	porting::app_global = app;

// 	Thread::setName("Main");

	try {
		app_dummy();
		main(0, nullptr);
	} catch (std::exception &e) {
// 		errorstream << "Uncaught exception in main thread: " << e.what() << std::endl;
		retval = -1;
	} catch (...) {
		// errorstream << "Uncaught exception in main thread!" << std::endl;
		retval = -1;
	}

	porting::cleanupAndroid();
	// infostream << "Shutting down." << std::endl;
	exit(retval);
}

/* handler for finished message box input */
/* Intentionally NOT in namespace porting */
/* TODO this doesn't work as expected, no idea why but there's a workaround   */
/* for it right now */
extern "C" {
	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_TextEntry_putMessageBoxResult(
		JNIEnv * env, jclass thiz, jstring textString)
	{
		if (porting::app_global->userData) {
			auto device = static_cast<irr::IrrlichtDevice*>(porting::app_global->userData);
			auto irrenv = device->getGUIEnvironment();
			auto element = irrenv->getFocus();
			if (element && element->getType() == irr::gui::EGUIET_EDIT_BOX) {
				auto editbox = static_cast<irr::gui::IGUIEditBox*>(element);
				const char* text = env->GetStringUTFChars(textString, NULL);
				auto wtext = BufferIO::DecodeUTF8s(text);
				editbox->setText(wtext.c_str());
				irrenv->removeFocus(editbox);
				irrenv->setFocus(editbox->getParent());
				irr::SEvent changeEvent;
				changeEvent.EventType = irr::EET_GUI_EVENT;
				changeEvent.GUIEvent.Caller = editbox;
				changeEvent.GUIEvent.Element = 0;
				changeEvent.GUIEvent.EventType = irr::gui::EGET_EDITBOX_CHANGED;
				editbox->getParent()->OnEvent(changeEvent);
				irr::SEvent enterEvent;
				enterEvent.EventType = irr::EET_GUI_EVENT;
				enterEvent.GUIEvent.Caller = editbox;
				enterEvent.GUIEvent.Element = 0;
				enterEvent.GUIEvent.EventType = irr::gui::EGET_EDITBOX_ENTER;
				editbox->getParent()->OnEvent(enterEvent);
				env->DeleteLocalRef(textString);
			}
		}
	}

	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_EpNativeActivity_putComboBoxResult(
		JNIEnv * env, jclass thiz, jint index) {
		if(porting::app_global->userData) {
			auto device = static_cast<irr::IrrlichtDevice*>(porting::app_global->userData);
			auto irrenv = device->getGUIEnvironment();
			auto element = irrenv->getFocus();
			if(element && element->getType() == irr::gui::EGUIET_COMBO_BOX) {
				auto combobox = static_cast<irr::gui::IGUIComboBox*>(element);
				combobox->setSelected(index);
				irr::SEvent changeEvent;
				changeEvent.EventType = irr::EET_GUI_EVENT;
				changeEvent.GUIEvent.Caller = combobox;
				changeEvent.GUIEvent.Element = 0;
				changeEvent.GUIEvent.EventType = irr::gui::EGET_COMBO_BOX_CHANGED;
				combobox->getParent()->OnEvent(changeEvent);
			}
		}
	}

	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_EpNativeActivity_pauseApp(
		JNIEnv * env, jclass thiz, jboolean pause) {
		if(ygo::gSoundManager) {
			ygo::gSoundManager->PauseMusic(pause);
		}
	}
}

namespace porting {

std::string internal_storage = "";
std::string working_directory = "";

android_app* app_global = nullptr;
JNIEnv*      jnienv = nullptr;
jclass       nativeActivity;

jclass findClass(std::string classname, JNIEnv* env = nullptr)
{
	env = env ? env : jnienv;
	if (env == 0) {
		return 0;
	}

	jclass nativeactivity = env->FindClass("android/app/NativeActivity");
	jmethodID getClassLoader =
	env->GetMethodID(nativeactivity,"getClassLoader",
					"()Ljava/lang/ClassLoader;");
	jobject cls =
	env->CallObjectMethod(app_global->activity->clazz, getClassLoader);
	jclass classLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID findClass =
			env->GetMethodID(classLoader, "loadClass",
					"(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName =
			env->NewStringUTF(classname.c_str());
	return (jclass) env->CallObjectMethod(cls, findClass, strClassName);
}

void initAndroid()
{
	porting::jnienv = NULL;
	JavaVM *jvm = app_global->activity->vm;
	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "Edopro NativeThread";
	lJavaVMAttachArgs.group = NULL;
#ifdef NDEBUG
	// This is a ugly hack as arm v7a non debuggable builds crash without this
	// printf ... if someone finds out why please fix it!
	// infostream << "Attaching native thread. " << std::endl;
#endif
	if ( jvm->AttachCurrentThread(&porting::jnienv, &lJavaVMAttachArgs) == JNI_ERR) {
		// errorstream << "Failed to attach native thread to jvm" << std::endl;
		exit(-1);
	}

	nativeActivity = findClass("io/github/edo9300/edopro/EpNativeActivity");
	if (nativeActivity == 0) {
		// errorstream <<
// 			"porting::initAndroid unable to find java native activity class" <<
// 			std::endl;
	}
}

void cleanupAndroid()
{
	JavaVM *jvm = app_global->activity->vm;
	jvm->DetachCurrentThread();
}

static std::string javaStringToUTF8(jstring js)
{
	std::string str;
	// Get string as a UTF-8 c-string
	const char *c_str = jnienv->GetStringUTFChars(js, NULL);
	// Save it
	str = c_str;
	// And free the c-string
	jnienv->ReleaseStringUTFChars(js, c_str);
	return str;
}

// Calls static method if obj is NULL
static std::string getAndroidPath(jclass cls, jobject obj, jclass cls_File,
		jmethodID mt_getAbsPath, const char *getter)
{
	// Get getter method
	jmethodID mt_getter;
	if (obj)
		mt_getter = jnienv->GetMethodID(cls, getter,
				"()Ljava/io/File;");
	else
		mt_getter = jnienv->GetStaticMethodID(cls, getter,
				"()Ljava/io/File;");

	// Call getter
	jobject ob_file;
	if (obj)
		ob_file = jnienv->CallObjectMethod(obj, mt_getter);
	else
		ob_file = jnienv->CallStaticObjectMethod(cls, mt_getter);

	// Call getAbsolutePath
	jstring js_path = (jstring) jnienv->CallObjectMethod(ob_file,
			mt_getAbsPath);

	return javaStringToUTF8(js_path);
}

void initializePathsAndroid()
{
 	// Get Environment class
 	jclass cls_Env = jnienv->FindClass("android/os/Environment");
 	// Get File class
 	jclass cls_File = jnienv->FindClass("java/io/File");
 	// Get getAbsolutePath method
 	jmethodID mt_getAbsPath = jnienv->GetMethodID(cls_File,
 				"getAbsolutePath", "()Ljava/lang/String;");
 
//  	path_cache   = getAndroidPath(nativeActivity, app_global->activity->clazz,
// 			cls_File, mt_getAbsPath, "getCacheDir");
	internal_storage = app_global->activity->internalDataPath;

	readConfigs();
// 	path_user    = path_storage + DIR_DELIM + PROJECT_NAME_C;
// 	path_share   = path_storage + DIR_DELIM + PROJECT_NAME_C;
}
void displayKeyboard(bool pShow) {
	// Attaches the current thread to the JVM.
	jint lResult;
	jint lFlags = 0;

	JavaVM* lJavaVM = app_global->activity->vm;
	JNIEnv* lJNIEnv = app_global->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = NULL;

	lResult = lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if(lResult == JNI_ERR) {
		return;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = app_global->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	// Retrieves Context.INPUT_METHOD_SERVICE.
	jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
	jfieldID FieldINPUT_METHOD_SERVICE =
		lJNIEnv->GetStaticFieldID(ClassContext,
								  "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
	jobject INPUT_METHOD_SERVICE =
		lJNIEnv->GetStaticObjectField(ClassContext,
									  FieldINPUT_METHOD_SERVICE);

	// Runs getSystemService(Context.INPUT_METHOD_SERVICE).
	jclass ClassInputMethodManager = lJNIEnv->FindClass(
		"android/view/inputmethod/InputMethodManager");
	jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(
		ClassNativeActivity, "getSystemService",
		"(Ljava/lang/String;)Ljava/lang/Object;");
	jobject lInputMethodManager = lJNIEnv->CallObjectMethod(
		lNativeActivity, MethodGetSystemService,
		INPUT_METHOD_SERVICE);

	// Runs getWindow().getDecorView().
	jmethodID MethodGetWindow = lJNIEnv->GetMethodID(
		ClassNativeActivity, "getWindow",
		"()Landroid/view/Window;");
	jobject lWindow = lJNIEnv->CallObjectMethod(lNativeActivity,
												MethodGetWindow);
	jclass ClassWindow = lJNIEnv->FindClass(
		"android/view/Window");
	jmethodID MethodGetDecorView = lJNIEnv->GetMethodID(
		ClassWindow, "getDecorView", "()Landroid/view/View;");
	jobject lDecorView = lJNIEnv->CallObjectMethod(lWindow,
												   MethodGetDecorView);

	if(pShow) {
		// Runs lInputMethodManager.showSoftInput(...).
		jmethodID MethodShowSoftInput = lJNIEnv->GetMethodID(
			ClassInputMethodManager, "showSoftInput",
			"(Landroid/view/View;I)Z");
		jboolean lResult = lJNIEnv->CallBooleanMethod(
			lInputMethodManager, MethodShowSoftInput,
			lDecorView, lFlags);
	} else {
		// Runs lWindow.getViewToken()
		jclass ClassView = lJNIEnv->FindClass(
			"android/view/View");
		jmethodID MethodGetWindowToken = lJNIEnv->GetMethodID(
			ClassView, "getWindowToken", "()Landroid/os/IBinder;");
		jobject lBinder = lJNIEnv->CallObjectMethod(lDecorView,
													MethodGetWindowToken);

		// lInputMethodManager.hideSoftInput(...).
		jmethodID MethodHideSoftInput = lJNIEnv->GetMethodID(
			ClassInputMethodManager, "hideSoftInputFromWindow",
			"(Landroid/os/IBinder;I)Z");
		jboolean lRes = lJNIEnv->CallBooleanMethod(
			lInputMethodManager, MethodHideSoftInput,
			lBinder, lFlags);
	}

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

void showInputDialog(const std::string& acceptButton, const  std::string& hint,
		const std::string& current, int editType)
{
	jmethodID showdialog = jnienv->GetMethodID(nativeActivity,"showDialog",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

	if (showdialog == 0) {
		assert("porting::showInputDialog unable to find java show dialog method" == 0);
	}

	jstring jacceptButton = jnienv->NewStringUTF(acceptButton.c_str());
	jstring jhint         = jnienv->NewStringUTF(hint.c_str());
	jstring jcurrent      = jnienv->NewStringUTF(current.c_str());
	jint    jeditType     = editType;

	jnienv->CallVoidMethod(app_global->activity->clazz, showdialog,
			jacceptButton, jhint, jcurrent, jeditType);
}

void showComboBox(const std::vector<std::string>& list) {
	jmethodID showbox = jnienv->GetMethodID(nativeActivity,"showComboBox",
		"([Ljava/lang/String;)V");

	jstring str;
	jobjectArray jlist = 0;
	jsize len = list.size();

	jlist = jnienv->NewObjectArray(len, jnienv->FindClass("java/lang/String"), 0);

	for(int i = 0; i < list.size(); i++) {
		str = jnienv->NewStringUTF(list[i].c_str());
		jnienv->SetObjectArrayElement(jlist, i, str);
	}

	jnienv->CallVoidMethod(app_global->activity->clazz, showbox, jlist);
}

float getDisplayDensity()
{
	static bool firstrun = true;
	static float value = 0;

	if (firstrun) {
		jmethodID getDensity = jnienv->GetMethodID(nativeActivity, "getDensity",
					"()F");

		if (getDensity == 0) {
			assert("porting::getDisplayDensity unable to find java getDensity method" == 0);
		}

		value = jnienv->CallFloatMethod(app_global->activity->clazz, getDensity);
		firstrun = false;
	}
	return value;
}

std::pair<int,int> getDisplaySize()
{
	static bool firstrun = true;
	static std::pair<int,int> retval;

	if (firstrun) {
		jmethodID getDisplayWidth = jnienv->GetMethodID(nativeActivity,
				"getDisplayWidth", "()I");

		if (getDisplayWidth == 0) {
			assert("porting::getDisplayWidth unable to find java getDisplayWidth method" == 0);
		}

		retval.first = jnienv->CallIntMethod(app_global->activity->clazz,
				getDisplayWidth);

		jmethodID getDisplayHeight = jnienv->GetMethodID(nativeActivity,
				"getDisplayHeight", "()I");

		if (getDisplayHeight == 0) {
			assert("porting::getDisplayHeight unable to find java getDisplayHeight method" == 0);
		}

		retval.second = jnienv->CallIntMethod(app_global->activity->clazz,
				getDisplayHeight);

		firstrun = false;
	}
	return retval;
}

/*
* implementation from https://github.com/minetest/minetest/blob/02a23892f94d3c83a6bdc301defc0e7ade7e1c2b/src/gui/modalMenu.cpp#L116
* with this patch applied to the engine https://github.com/minetest/minetest/blob/02a23892f94d3c83a6bdc301defc0e7ade7e1c2b/build/android/patches/irrlicht-touchcount.patch
*/
bool transformEvent(const irr::SEvent & event) {
	static irr::core::position2di m_pointer = irr::core::position2di(0, 0);
	auto device = static_cast<irr::IrrlichtDevice*>(porting::app_global->userData);
	if(event.EventType == irr::EET_TOUCH_INPUT_EVENT) {
		irr::SEvent translated;
		memset(&translated, 0, sizeof(irr::SEvent));
		translated.EventType = irr::EET_MOUSE_INPUT_EVENT;

		translated.MouseInput.X = event.TouchInput.X;
		translated.MouseInput.Y = event.TouchInput.Y;
		translated.MouseInput.Control = false;

		switch(event.TouchInput.touchedCount) {
			case 1:	{
				switch(event.TouchInput.Event) {
					case irr::ETIE_PRESSED_DOWN:
						m_pointer = irr::core::position2di(event.TouchInput.X, event.TouchInput.Y);
						translated.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
						translated.MouseInput.ButtonStates = irr::EMBSM_LEFT;
						irr::SEvent hoverEvent;
						hoverEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
						hoverEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
						hoverEvent.MouseInput.X = event.TouchInput.X;
						hoverEvent.MouseInput.Y = event.TouchInput.Y;
						device->postEventFromUser(hoverEvent);
						break;
					case irr::ETIE_MOVED:
						m_pointer = irr::core::position2di(event.TouchInput.X, event.TouchInput.Y);
						translated.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
						translated.MouseInput.ButtonStates = irr::EMBSM_LEFT;
						break;
					case irr::ETIE_LEFT_UP:
						translated.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
						translated.MouseInput.ButtonStates = 0;
						// we don't have a valid pointer element use last
						// known pointer pos
						translated.MouseInput.X = m_pointer.X;
						translated.MouseInput.Y = m_pointer.Y;
						break;
					default:
						return true;
				}
				break;
			}
			case 2: {
				if(event.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
					translated.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
					translated.MouseInput.ButtonStates = irr::EMBSM_LEFT | irr::EMBSM_RIGHT;
					translated.MouseInput.X = m_pointer.X;
					translated.MouseInput.Y = m_pointer.Y;
					device->postEventFromUser(translated);

					translated.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
					translated.MouseInput.ButtonStates = irr::EMBSM_LEFT;

					device->postEventFromUser(translated);
				}
				return true;
			}
			case 3:	{
				if(event.TouchInput.Event == irr::ETIE_LEFT_UP) {
					translated.EventType = irr::EET_KEY_INPUT_EVENT;
					translated.KeyInput.Control = true;
					translated.KeyInput.PressedDown = false;
					translated.KeyInput.Key = irr::KEY_KEY_O;
					device->postEventFromUser(translated);
				}
				return true;
			}
			default:
				return true;
		}

		// check if translated event needs to be preprocessed again
		if(transformEvent(translated)) {
			return true;
		}

		bool retval = device->postEventFromUser(translated);;

		if(event.TouchInput.Event == irr::ETIE_LEFT_UP) {
			// reset pointer
			m_pointer = irr::core::position2di(0, 0);
		}
		return retval;
	}
	return false;
}

void readConfigs() {
	std::string path = internal_storage + "/working_dir";
	struct stat buffer;
	if(stat(path.c_str(), &buffer) == 0) {
		LOGI("working_dir found");
		std::ifstream confs(path);
		std::getline(confs, working_directory);
		LOGI("Working directory: %s", working_directory.c_str());
	}
	if(working_directory.empty()){
		LOGI("working_dir not found");
		exit(1);
	}
	working_directory += "/";
}

int getLocalIP() {
	jmethodID getIP = jnienv->GetMethodID(nativeActivity, "getLocalIpAddress",
											   "()I");
	if(getIP == 0) {
		assert("porting::getLocalIP unable to find java getLocalIpAddress method" == 0);
	}
	int value = jnienv->CallIntMethod(app_global->activity->clazz, getIP);
	return value;
}

void launchWindbot(const std::string& args) {
	jmethodID launchwindbot = jnienv->GetMethodID(nativeActivity, "launchWindbot",
											   "(Ljava/lang/String;)V");

	if(launchwindbot == 0) {
		assert("porting::launchWindbot unable to find java launchWindbot method" == 0);
	}

	jstring jargs = jnienv->NewStringUTF(args.c_str());

	jnienv->CallVoidMethod(app_global->activity->clazz, launchwindbot,
						   jargs);
}

void setTextToClipboard(const wchar_t* text) {
	jmethodID setClip = jnienv->GetMethodID(nativeActivity, "setClipboard",
											   "(Ljava/lang/String;)V");

	if(setClip == 0) {
		assert("porting::setTextToClipboard unable to find java setClipboard method" == 0);
	}
	jstring jargs = jnienv->NewStringUTF(BufferIO::EncodeUTF8s(text).c_str());

	jnienv->CallVoidMethod(app_global->activity->clazz, setClip, jargs);
}

const wchar_t* getTextFromClipboard() {
	static std::wstring text;
	jmethodID getClip = jnienv->GetMethodID(nativeActivity, "getClipboard",
										  "()Ljava/lang/String;");
	if(getClip == 0) {
		assert("porting::getTextFromClipboard unable to find java getClipboard method" == 0);
	}
	jstring js_clip = (jstring)jnienv->CallObjectMethod(app_global->activity->clazz, getClip);
	text = BufferIO::DecodeUTF8s(javaStringToUTF8(js_clip));
	return text.c_str();

}
}