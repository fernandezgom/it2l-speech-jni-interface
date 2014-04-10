import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;

public class Italk2learn {
	
	
	//JLF: Send chunks of audio to Speech Recognition engine each 5 seconds
    public native void sendNewAudioChunk(byte[] buf);
    //JLF: Open the listener and retrieves true if the operation was right. It is executed when the user is logged in the platform and change the exercise
    public native boolean initSpeechRecognitionEngine();
    //JLF: Close the listener and retrieves the whole transcription. It is executed each time the exercise change to another
    public native String close();
	
	//JLF: Send chunks of audio to Speech Recognition engine
	public void sendNewChunk(byte[] data) {
		System.out.println("Sending data from Java!");
		try {
			this.sendNewAudioChunk(data);
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		} 
	}
	
	//JLF:Open the listener and retrieves true if the operation was right
	public boolean initSpeechRecognition() {
		System.out.println("Open Listener from Java!");
		boolean result=false;
		try {
			result=this.initSpeechRecognitionEngine();
			System.out.println(result);
			return result;
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		} 
		return result;
	}
	
	//JLF:Close the listener and retrieves the whole transcription
	public String closeEngine() {
		System.out.println("Close Listener from Java!");
		String result="";
		try {
			result=this.close();
			System.out.println(result);
			return result;
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		} 
		return result;
	}
	
	// JLF: Retrieves data from ASRResult on real time
	public String realTimeSpeech(String text) {
	    System.out.println(text);
	    return text;
	}
	
	public static void main(String[] args) {
		File f=new File("gemma1.wav");
		long l=f.length();
		System.out.println("the file is " + l + " bytes long");
		FileInputStream finp = null;
		byte[] b=new byte[(int)l];
		try {
			finp = new FileInputStream(f);
			int i;
			i=finp.read(b);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		byte[] aux1=Arrays.copyOfRange(b, 0, (int)l/4);
		byte[] aux2=Arrays.copyOfRange(b, ((int)l/4)+1, (int)l/2);
		byte[] aux3=Arrays.copyOfRange(b, ((int)l/2)+1, (int)(3*l)/4);
		byte[] aux4=Arrays.copyOfRange(b, ((int)(3*l)/4), (int)l-1);
		try {
			Italk2learn engine= new Italk2learn();
			if (engine.initSpeechRecognition()){
				engine.sendNewAudioChunk(aux1);
				engine.sendNewAudioChunk(aux2);
				engine.sendNewAudioChunk(aux3);
				engine.sendNewAudioChunk(aux4);
			}
			engine.closeEngine();
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	static {
		try {
			System.loadLibrary("iT2L");
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		}
	}

}
