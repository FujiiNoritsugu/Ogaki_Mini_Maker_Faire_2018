package sensor.client;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * 学習用プログラム
 * @author fujii
 *
 */
public class SensorClient {

	Socket socket;
	BufferedInputStream is;

	Socket ts;
	BufferedOutputStream ts_out;
	BufferedReader ts_in;
	Queue <Integer> queue;
	
	public SensorClient(Queue <Integer> queue){
		this.queue = queue;
	}
	
	public static void main(String [] args){
		// キューを作成する
		Queue <Integer> queue = new ConcurrentLinkedQueue<Integer>();
		SensorClient sensorClient = new SensorClient(queue);
		ServoClient servoClient = new ServoClient(queue);
		// サーボ側は別スレッドで処理する
		new Thread(servoClient).start();
		
		String de0ip = null;
		de0ip = "192.168.1.17";
		String tensorIp = "localhost";
		sensorClient.readData(de0ip, tensorIp);
	}

	private int count = 0;
	
	private void readData(String de0ip, String tensorIp){
		byte [] buff = new byte[4*1000];		
		try{
			socket = new Socket(de0ip, 24);
			is = new BufferedInputStream((socket.getInputStream()));
			// 音認識用のソケットも作成する
			ts = new Socket(tensorIp, 5000);
			ts_out = new BufferedOutputStream(ts.getOutputStream());
			ts_in = new BufferedReader(new InputStreamReader((ts.getInputStream())));
			
			while(true){
				int result = is.read(buff);
				if(result == -1){
					System.out.println("stream reach end");
				}else{


					ByteBuffer bb = ByteBuffer.wrap(buff);
					bb.order(ByteOrder.LITTLE_ENDIAN);
					List<Integer> dataList = new ArrayList<Integer>();					
					for(int i = 0; i < 1000; i++){
						dataList.add(bb.getInt(i * 4));
					}

					List <String> outputList = new ArrayList<String>();
					boolean isOutput = false;
					for(int i = 0; i < dataList.size(); i++){
						int value = dataList.get(i);
						int sensor1 = 0x00000fff & value;
						int sensor2 = ((0x00fff000 & value) >> 12);
						if(sensor1 >= 1000){
							isOutput = true;
						}
							//outputList.add(dataName + " 1:" + sensor1 + " 2:" + sensor2);
						outputList.add(String.valueOf(sensor1));
					}
					
					// １０００以上の値を計測した場合のみデータをソケット出力する
					if(isOutput){
						System.out.println(String.format(" count =  %d", ++ count));
						ts_out.write(String.join(",", outputList.toArray(new String[0])).getBytes());
						ts_out.flush();
						// 音認識の結果を受信する
						byte[] rec = new byte[1];
						String result_data = ts_in.readLine();
						System.out.println("result_data = " + result_data);
						 queue.add(Integer.parseInt(result_data));
					}
				}
			}
		}catch(Throwable th){
			th.printStackTrace();
		}finally{
			try {
				if(is != null){is.close();}
				if(socket != null){socket.close();}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
