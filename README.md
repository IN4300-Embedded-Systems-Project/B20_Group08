# 🚀 B20_Group08 - Intruder Detection System using Multi-Beam Lasers

## 📌 Overview
Our **Intruder Detection System** is a **proactive security solution** designed to detect unauthorized access in **real-time** and provide **instant alerts**. Unlike conventional security systems that rely on **passive monitoring**, this system uses **multi-beam lasers** to form an **invisible security grid**, ensuring immediate detection and response.

---

## ⚙️ How It Works
- **Laser Grid Detection**: Laser diodes (**KY-008 module**) and **light-dependent resistors (LDRs)** detect intrusions when a beam is interrupted.  
- **Arduino-Based Control**: The **Arduino Uno** processes signals and triggers an **alarm via a buzzer**.  
- **Instant Notifications**: A **SIM800L GSM module** sends an **SMS alert** to the user.  
- **Real-Time Display**: An **OLED I2C display** shows the system's real-time status.  
- **Cloud & Mobile Integration**:  
  - Detected intrusions are logged in a **Firebase database** via a **Node.js backend** hosted on **AWS**.  
  - A **mobile app** provides **real-time push notifications** and access to **detailed logs**.  

---

## 🏢 Applications
This system is ideal for securing:  
✅ **Homes**  
✅ **Offices**  
✅ **Warehouses**  
✅ **Restricted areas**  

Organizations can use it to monitor access points **after working hours**, ensuring enhanced security with **real-time alerts and cloud-based monitoring**.  

---

## 🔩 Components Used
| Component               | Description                                |
|-------------------------|--------------------------------------------|
| **Arduino Uno**         | Microcontroller for system control         |
| **KY-008 Laser Module** | Generates laser beams for the security grid |
| **LDR (Light Dependent Resistor)** | Detects laser beam interruptions |
| **SIM800L GSM Module**  | Sends SMS alerts to the user              |
| **OLED I2C Display**    | Displays real-time system status          |
| **Buzzer**             | Triggers an alarm when an intrusion is detected |

---

## 🔥 Features
✔ **Real-time intrusion detection**  
✔ **Instant SMS and mobile push notifications**  
✔ **OLED display for real-time status updates**  
✔ **Cloud-based monitoring via Firebase and AWS**  
✔ **Mobile app for log access and alerts**  
✔ **Cost-effective and scalable security solution**  

This project enhances **security infrastructures** by combining **laser-based detection, IoT communication, and cloud technology** to deliver a **reliable and efficient intrusion detection system**.  

---

## 📊 Block Diagram
Below is the block diagram illustrating the working of our **Intruder Detection System**:  

![Block Diagram](https://github.com/IN4300-Embedded-Systems-Project/B20_Group08/blob/main/Assets/Group08_block_diagram.png)

---

## 🔌 Circuit Diagram
The circuit diagram represents the wiring and connections of our **Intruder Detection System**:  

![Circuit Diagram](https://github.com/IN4300-Embedded-Systems-Project/B20_Group08/blob/main/Assets/Group_08_circuit_diagram.png)

---

## 🛠️ Tech Stack
- **Hardware:** Arduino Uno, KY-008 Laser Module, LDRs, SIM800L, OLED I2C Display, Buzzer  
- **Backend:** Node.js, Firebase, AWS  
- **Mobile App:** Integrated with Firebase for real-time notifications  
