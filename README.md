# HomeService

IoT Temelli Akıllı Ev Sistemi
Bu proje, ESP32 mikrodenetleyicisi kullanılarak geliştirilmiş, sürekli erişim sağlayan bir IoT tabanlı akıllı ev sistemidir. Sistem, esnek ve çok katmanlı bir haberleşme altyapısına sahiptir:

Bulut Bağlantısı: WiFi üzerinden Blynk Cloud kullanılarak cihazlar kontrol edilebilir ve izlenebilir.
Yerel Ağ Modu: İnternet bağlantısı kesildiğinde sistem yerel ağ üzerinden bir web sunucusu oluşturur ve kullanıcı erişimini sürdürür.
AP Modu: Modem bağlantısı tamamen kaybolursa, sistem bir erişim noktası (Access Point) oluşturarak aynı kullanıcı arayüzüyle kontrol imkanı sağlar.
Özellikler: LED, servo motor, DC motor kontrolü ve DHT22 ile sıcaklık/nem izleme. Üç aşamalı haberleşme yapısıyla kesintisiz bir akıllı ev deneyimi sunar.
