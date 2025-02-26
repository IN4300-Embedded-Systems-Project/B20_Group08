import React, { useState } from 'react';
import { View, Text, TouchableOpacity, StyleSheet } from 'react-native';
import { ref, set } from 'firebase/database';
import { db } from '../Firebaseconfiguration'; // Ensure Firebase is set up

export default function Home() {
  const [alarmStatus, setAlarmStatus] = useState(false);
  const [triggeralarmStatus, settriggerAlarmStatus] = useState(false);


  // Toggle Alarm Status in Firebase
  const toggleAlarm = () => {
    const newStatus = !alarmStatus;
    set(ref(db, 'alarmStatus/'), { active: newStatus });
    setAlarmStatus(newStatus);
  };

  // Trigger Alarm
  const triggerAlarm = () => {
    const newStatus = !triggeralarmStatus;
    set(ref(db, 'alarmTrigger/'), { triggered: newStatus });
settriggerAlarmStatus(newStatus);
  };

  return (
    <View style={styles.container}>
      <Text style={styles.heading}>Security Alarm Control</Text>

      <TouchableOpacity
        style={[styles.button, alarmStatus ? styles.alarmOn : styles.alarmOff]}
        onPress={toggleAlarm}
      >
        <Text style={styles.buttonText}>{alarmStatus ? 'Turn Off Alarm' : 'Turn On Alarm'}</Text>
      </TouchableOpacity>

      <TouchableOpacity style={[styles.triggerButton ,, triggeralarmStatus ? styles.alarmOn : styles.alarmOff]} 
      onPress={triggerAlarm}>
        <Text style={styles.buttonText}>{triggeralarmStatus ? 'Trigger Alarm' : 'Not Trigger Alarm'}</Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, justifyContent: 'center', alignItems: 'center', padding: 20 },
  heading: { fontSize: 22, fontWeight: 'bold', marginBottom: 20 },
  button: { padding: 15, borderRadius: 8, marginVertical: 10, width: '80%', alignItems: 'center' },
  alarmOn: { backgroundColor: 'red' },
  alarmOff: { backgroundColor: 'green' },
  triggerButton: { backgroundColor: 'blue', padding: 15, borderRadius: 8, width: '80%', alignItems: 'center' },
  buttonText: { color: 'white', fontSize: 16, fontWeight: '600' },
});
