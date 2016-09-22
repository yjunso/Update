using UnityEngine;
using System.Collections;
using WebSocketSharp;
using System.IO;

public class VrTrackerUser : MonoBehaviour {
	
	private WebSocket ws;
	private bool detecting = false;
	private Vector3 position;
	private Vector3 previousPosition;
	public string GatewayIP = ""; // Set your gateway IP here
	public string WebsocketPort = "8001"; // Set your port here
	public float scale = 0.01f;
	public float x_offset = 0.0f;
	public float y_offset = 0.0f;
	public float z_offset = 0.0f;
	public Transform userCamera; 
	
	
	void Start () {
		
		// Retrieve the main camera
		//userCamera
		//userCamera = Transform;
		
		// Create and Open the websocket
		ws = new WebSocket("ws://"+ GatewayIP + ":" + WebsocketPort);
		ws.OnOpen += OnOpenHandler;
		ws.OnMessage += OnMessageHandler;
		ws.OnClose += OnCloseHandler;
		
		ws.ConnectAsync();	
	}
	
	private void OnOpenHandler(object sender, System.EventArgs e) {
		Debug.Log("Connected to Gateway!");
		WebsocketDisplay.connectionText = "Connected to Gateway!";
		ws.SendAsync ("user-2", OnSendComplete);
	}
	
	private void OnMessageHandler(object sender, MessageEventArgs e) {
		// Split the position in XYZ
		string[] temp = e.Data.Split(":"[0]);
		
		position.x = float.Parse(temp[0])+x_offset;
		position.y = float.Parse(temp[2])+y_offset;
		position.z = float.Parse(temp[1])+z_offset;
		Debug.Log ("Position received : " + position);
		// Here you can add some post treatment on the position (remove weird datas, add a Kalman filter, smoothen the curve...
			
		/*if (Mathf.Abs (previousPosition [0] - position [0]) > 20 || Mathf.Abs (previousPosition [1] - position [1]) > 20 || Mathf.Abs (previousPosition [2] - position [2]) > 20) {
			Debug.LogError ("Incoherent 3D position received");
		} else {
			position = previousPosition;
			Debug.Log (position);
		}*/
	}

	private void OnCloseHandler(object sender, CloseEventArgs e) {
		Debug.Log("Connection to Gateway closed for this reason: " + e.Reason);
		WebsocketDisplay.connectionText = "Connection closed: " + e.Reason;
	}

	private void OnSendComplete(bool success) {

	}
	
	// Update is called once per frame
	void Update () {
	//	Debug.Log("Up");
		Vector3 pos = new Vector3(position.x, position.y, position.z);
		userCamera.transform.position = pos*scale;
	}

	void OnApplicationQuit() {
		Debug.Log("Application ending after " + Time.time + " seconds");
		ws.Close();
	}
	
}
