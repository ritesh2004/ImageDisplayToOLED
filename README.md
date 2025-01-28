# ImageDisplayToOLED
An experimental project which takes picture and display it in a OLED display. 

---

**Back to Building After a Short Break! 🚀**  

This time, I worked on a small project with the **ESP32-CAM**, focusing on capturing an image and displaying it on an **OLED screen** that only supports two colors. The challenge here was to process grayscale image data and convert it into a binary format suitable for the OLED display.  

### 🔹 Workflow:  
1️⃣ The ESP32-CAM captures an image using its camera module.  
2️⃣ A web server running on the ESP32 allows image retrieval via a browser.  
3️⃣ The captured grayscale image is downscaled and converted into a black-and-white format for OLED display.  
4️⃣ Pressing a button triggers image capture and display on the OLED.  

The main goal of this project was to experiment with **image processing, embedded development, and I2C communication**—a great learning experience!  

Let me know your thoughts or if you have any suggestions to improve it. Always open to learning from the community! 😊  

#EmbeddedSystems #IoT #ESP32CAM #LearningByDoing #HardwareDevelopment