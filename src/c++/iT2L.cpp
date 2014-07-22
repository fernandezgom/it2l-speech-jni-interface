/* CPPDOC_BEGIN_EXCLUDE */

#include <jni.h>
#include <iostream>
#include <string>

#ifdef WIN32
#include <process.h>
#include <windows.h>
#include <sys/stat.h>
#define strcasecmp stricmp
#endif

#include "ASREngine.h"
#include <time.h>


using namespace std;
using namespace ASR;

//==========================================================================
// class MyListener
//
// Handles status messages and results from the engine
//==========================================================================
class MyListener : public ASREngineListener
{
public:
	virtual ~MyListener() {};
	void setFileName(string path, bool createXMLAtSource=false) { m_Path = path; m_createXMLAtSource=createXMLAtSource; }

	// the three required listening methods of ASREngineListener:

	virtual void status(STATUS status);

	virtual void asrResult(RESULTTYPE type,
								  vector<ASREngineResult>& utterance,
								  string speaker);

	virtual void xmlResult(const string& xml);

	string myResult;

	string getResult() {return myResult;}

	string liveResult;

	string getLiveResult() {return liveResult;}

	
private:

	string m_Path;
	bool m_createXMLAtSource;	// create resulting XML where the input-file is

	string getXmlFileName() const
	{
		char fname[255];
		char dirname[255];
		char fileWithPath[600];
		char drivename[5];

		ASREngine::splitpath
			(m_Path.c_str(), drivename, dirname, fname, NULL);

		fileWithPath[0]='\0';

		// depending on this flag, the xml file will be in current directory,
		// or in the directory where the input file resides
		if(m_createXMLAtSource)
			sprintf(fileWithPath, "%s%s", drivename, dirname);

		// same name as audio file, extension .xml
		strcat(fileWithPath, fname);
		strcat(fileWithPath, ".xml");
		return string(fileWithPath);
	}

	//void optionalBackup(const string& filename) const;
};


// Handle final XML result from engine
void MyListener::xmlResult(const string& xml)
{
	string filename = getXmlFileName();
	cout << endl << "Writing XML result to: " << filename << endl;
	//ofstream os;
	//os.open(filename.c_str(), ios_base::out);
	//os << xml;
	//os.close();
	myResult=xml;
	// optionally save a backup copy of input file and xml file
	//optionalBackup(filename);
}


extern "C" {
	//JLF: Global variables
	ASREngine* asr;
	MyListener listener;
	static JavaVM *jvm = 0;
    static jobject surface;

//JLF: Method that sends data to the engine on real time	
JNIEXPORT void JNICALL Java_Italk2learn_sendNewAudioChunk(JNIEnv * env, jobject obj, jbyteArray sample) {
	printf("Send new chunk from C++!\n");
#ifdef __cplusplus
	printf("__cplusplus is defined\n");
#else
	printf("__cplusplus is NOT defined\n");
#endif

	jbyte *inBytes = env->GetByteArrayElements(sample, 0);
   
    jsize length = env->GetArrayLength( sample);
	
	printf("from Java: length =%d, bytes 0..3: %d %d %d %d\n",length,inBytes[0],inBytes[1],inBytes[2],inBytes[3]);
    asr->sendData((unsigned char*)inBytes,length);
   
    env->ReleaseByteArrayElements(sample, inBytes, 0); // release resources

}


//JLF: Method that opens a listener and initializes the engine
JNIEXPORT jboolean JNICALL Java_Italk2learn_initSpeechRecognitionEngine(JNIEnv * jenv, jobject surfaceView, jstring server, jint instance, jstring languageCode, jstring model) {
	printf("Init ASR Engine from C++!\n");
#ifdef __cplusplus
	printf("__cplusplus is defined\n");
#else
	printf("__cplusplus is NOT defined\n");
#endif
	jint rs = jenv->GetJavaVM(&jvm);
    surface = jenv->NewGlobalRef(surfaceView);

	jboolean b=true;
	string servername = jenv ->GetStringUTFChars(server,0);
	int instanceNum = (int) instance;
	asr = new ASREngine (servername, instanceNum);

	asr->addListener(&listener);

	printf("Listener declared\n");
	// start engine
	string language (jenv ->GetStringUTFChars(languageCode,0));
	string domain   ("broadcast-news");
	string subdomain(jenv ->GetStringUTFChars(model,0));
	string mode     ("accurate");

	asr->initialize(language, domain, subdomain, mode, 44100);

	printf("ASR Initialized\n");
	//Release memory
	//jenv -> ReleaseStringUTFChars
	return true;

}

//JLF:Close the listener and retrieves the whole transcription
JNIEXPORT jstring JNICALL Java_Italk2learn_close(JNIEnv * env, jobject) {
    printf("Closing listener from C++\n");
    asr->endOfData();
  
    asr->waitForJobDone();
    
    string result=listener.getResult();
    printf(result.c_str());
    delete asr;
    asr=0;
    return env->NewStringUTF(result.c_str());
}

// Handle status messages from engine
void MyListener::status(STATUS status)
{
	switch(status)
	{
		case STATUS_ERROR:
			cerr << endl << "Error" << endl;
			break;

		case STATUS_JOBDONE:
			break;
	}
}


// Handle intermediate results from engine
void MyListener::asrResult(
		RESULTTYPE type,
		vector<ASREngineResult>& utterance, string speaker)
{
	printf("Result_real_time\n");
	JNIEnv *env = 0;
    jint rs = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (rs == JNI_OK)
        cout <<  "";
    else if (rs == JNI_EDETACHED) {
        cout << "EDETACHED\n";
        if (jvm->AttachCurrentThread((void **) &env, NULL) < 0) {
            cout << "Failed to get the environment using AttachCurrentThread()\n";
        } else {
          // Success : Attached and obtained JNIEnv!
            cout << "success\n";
        }
    }
    else if (rs == JNI_EVERSION)
         cout << "EVERSION\n";
    else
         cout << "ELSE\n";

	switch(type)
	{
		case ASR_RESULT:
			cout << "--- SEGMENT RESULT ---";
			break;

		case TRANSLATION_RESULT:
			cout << "--- TRANSLATION ---";
			break;

		default:
			return;
	}

	if (!speaker.empty())
		cout << " speaker: " << speaker;
		cout << endl;

	for (vector<ASREngineResult>::iterator itr = utterance.begin();
		  itr != utterance.end();
		  itr++)
	{
		//cout << itr->word << "[" << itr->offset << "] ";
		printf("Word: ");
		string res=itr->word;
		printf(res.c_str());
		jstring jstr = env->NewStringUTF(res.c_str());
		jclass clazz = env->GetObjectClass(surface);
		if (!clazz) printf("classfail\n");
		jmethodID realTimeSpeech = env->GetMethodID(clazz, "realTimeSpeech", "(Ljava/lang/String;)Ljava/lang/String;");
		if (!realTimeSpeech) printf("methodfail\n");
		jobject result = env->CallObjectMethod(surface, realTimeSpeech, jstr);
	}

	cout << endl;
}

}

/* CPPDOC_END_EXCLUDE */
