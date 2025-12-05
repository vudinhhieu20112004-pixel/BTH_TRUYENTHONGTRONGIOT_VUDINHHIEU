#define BUTTON_PIN 2
#define LED_PIN 13

unsigned long pressStart = 0;
unsigned long lastReleaseTime = 0;
bool buttonState = HIGH;
bool lastButtonState = HIGH;
bool configMode = false;

int clickCount = 0;
int baudSelection = 9600;  // mặc định

void blinkLED(int times, int fast = 0) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(fast ? 100 : 300);
    digitalWrite(LED_PIN, LOW);
    delay(fast ? 100 : 300);
  }
}

void setBaudRate(int baud) {
  Serial.print("UART da cai dat: ");
  Serial.println(baud);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(baudSelection);
  Serial.println("Khoi dong o che do NORMAL");
}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);

  // --- PHÁT HIỆN NHẤN ---
  if (buttonState == LOW && lastButtonState == HIGH) {
    pressStart = millis();
  }

  // --- PHÁT HIỆN NHẢ ---
  if (buttonState == HIGH && lastButtonState == LOW) {
    unsigned long pressDuration = millis() - pressStart;

    if (pressDuration < 3000) {  
      // Đây là 1 lần nhấn ngắn
      if (configMode) {
        clickCount++;
        lastReleaseTime = millis();
      }
    }
  }

  // --- NHẤN GIỮ >3s: đổi chế độ ---
  if (buttonState == LOW && (millis() - pressStart > 3000) && lastButtonState == LOW) {
    configMode = !configMode;
    clickCount = 0;  // reset đếm

    if (configMode) {
      blinkLED(5, 1);
      Serial.println("=== MODE CAU HINH UART ===");
    } else {
      blinkLED(3, 1);
      Serial.println("=== TRO VE CHE DO HOAT DONG ===");
    }

    // Chờ thả nút trước khi cho phép nhấn tiếp
    while (digitalRead(BUTTON_PIN) == LOW);

    delay(300);
  }

  // --- XỬ LÝ CLICK TRONG CONFIG MODE ---
  if (configMode && clickCount > 0 && millis() - lastReleaseTime > 1200) {
    // nếu 1.2s không nhấn thêm -> chốt lựa chọn
    if (clickCount == 1) {
      baudSelection = 9600;
      blinkLED(1);
      setBaudRate(9600);
    } else if (clickCount >= 2) {
      baudSelection = 115200;
      blinkLED(2);
      setBaudRate(115200);
    }

    clickCount = 0;
    Serial.println("Nhan giu 3s de thoat config...");
  }

  lastButtonState = buttonState;

  // --- NORMAL MODE (đặt code chính ở đây) ---
  if (!configMode) {
    // code chính khi hoạt động bình thường
  }
}
