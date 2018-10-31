package sensor.client;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Queue;

public class ServoClient implements Runnable {

	Socket socket;
	PrintWriter out;

	Queue<Integer> queue;

	public ServoClient(Queue<Integer> queue) {
		this.queue = queue;

		try {
			socket = new Socket("192.168.1.17", 23);
			out = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	public static final int HIGH = 180;
	public static final int MIDDLE = 135;
	public static final int LOW = 90;
	
	@Override
	public void run() {
		while (true) {
			try {
				Integer type = queue.poll();
				int eyeblow = 0;
				int lips = 0;
				if (type != null) {
					System.out.println("type = " + type);
					switch (type) {
					// 喜ぶ
					case 0:
						eyeblow = HIGH;
						lips = HIGH;
						break;
					// 怒る
					case 1:
						eyeblow = HIGH;
						lips = LOW;
						break;
					// 泣く
					case 2:
						eyeblow = LOW;
						lips = LOW;
						break;
					// 笑う
					case 3:
						eyeblow = LOW;
						lips = HIGH;
						break;
					default:
						return;
					}
					sendData(eyeblow, lips);
					// 3秒後に元に戻す
					Thread.sleep(3 * 1000);
					sendData(MIDDLE, MIDDLE);
				}
			} catch (Throwable th) {
				th.printStackTrace();
			}
		}
	}
	
	private void sendData(int angle1, int angle2){
		System.out.println("eyeblow = " + angle1 + " lips = " + angle2);
		int value = 0xffffffff & (angle1 | (angle2 << 8));
		String data = String.format("d%05d", value);
		out.print(data);
		out.flush();
	}
}
