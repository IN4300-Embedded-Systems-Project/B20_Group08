import React, { useEffect, useState, useRef } from 'react';
import { View, Text, ScrollView, StyleSheet, TouchableOpacity } from 'react-native';
import { ref, onValue, set } from 'firebase/database';
import { db } from '../Firebaseconfiguration';
import * as Notifications from 'expo-notifications';
import { Audio } from 'expo-av';

interface Post {
  id: string;
  lasertripped: number;
  timestamp: string;
}

export default function Data() {
  const [data, setData] = useState<Post[]>([]);
  const [alarmSound, setAlarmSound] = useState<Audio.Sound | null>(null);
  const [notificationId, setNotificationId] = useState<string | null>(null);
  const [lastAlertId, setLastAlertId] = useState<string | null>(null);
  const scrollViewRef = useRef<ScrollView>(null);
  const hasLoadedOnce = useRef(false);

  useEffect(() => {
    async function setupAudio() {
      try {
        await Audio.setAudioModeAsync({
          playsInSilentModeIOS: true,
          staysActiveInBackground: true,
          shouldDuckAndroid: true,
        });
      } catch (error) {
        console.error('Error setting up audio:', error);
      }
    }
    setupAudio();
  }, []);

  useEffect(() => {
    const fetchRef = ref(db, 'data/');
    const unsubscribe = onValue(fetchRef, (snapshot) => {
      const fetchedData = snapshot.val();
      if (fetchedData) {
        const newPosts: Post[] = Object.keys(fetchedData).map((key) => ({
          id: key,
          ...fetchedData[key],
        }));
        setData(newPosts);

        if (hasLoadedOnce.current) {
          handleAlarm(newPosts);
        } else {
          hasLoadedOnce.current = true;
        }
      } else {
        setData([]);
      }
    });
    return () => unsubscribe();
  }, []);

  const handleAlarm = async (newPosts: Post[]) => {
    if (newPosts.length > 0) {
      const lastPost = newPosts[newPosts.length - 1];
      if (lastAlertId !== lastPost.id) {
        setLastAlertId(lastPost.id);
        await playAlarmSound();
        triggerNotification();
        scrollViewRef.current?.scrollToEnd({ animated: true });
      }
    }
  };

  async function playAlarmSound() {
    try {
      console.log('Loading alarm sound...');
      
      // Load and play sound directly without using Asset
      const { sound } = await Audio.Sound.createAsync(
        require('../assets/alarm.mp3'),
        { shouldPlay: true }
      );
      
      setAlarmSound(sound);
      console.log('Playing alarm sound...');
      
      // Add a listener for when playback finishes
      sound.setOnPlaybackStatusUpdate((status) => {
        if (status.isLoaded && status.didJustFinish) {
          sound.unloadAsync(); // Unload when finished to prevent memory leaks
        }
      });
      

    } catch (error) {
      console.error('Error playing alarm sound:', error);
    }
  }

  const triggerNotification = async () => {
    try {
      const id = await Notifications.scheduleNotificationAsync({
        content: {
          title: 'Security Alert!',
          body: 'A new alarm has been triggered!',
          sound: 'default', // Use the default sound instead of custom
          vibrate: [500, 500, 500],
        },
        trigger: null,
      });
      setNotificationId(id);
    } catch (error) {
      console.error('Error triggering notification:', error);
    }
  };

  const stopAlarm = async () => {
    try {
      await set(ref(db, 'reset/'), true);
      
      if (alarmSound) {
        await alarmSound.stopAsync();
        await alarmSound.unloadAsync();
        setAlarmSound(null);
      }
      if (notificationId) {
        await Notifications.dismissNotificationAsync(notificationId);
        setNotificationId(null);
      }
      setLastAlertId(null);
    } catch (error) {
      console.error('Error stopping alarm:', error);
    }
  };

  return (
    <ScrollView contentContainerStyle={styles.container} ref={scrollViewRef}>
      <Text style={styles.heading}>Security Updates</Text>
      {data.length === 0 ? (
        <Text style={styles.noData}>No security updates available.</Text>
      ) : (
        data.map((post) => (
          <View key={post.id} style={styles.postContainer}>
            <Text style={styles.postTitle}>{post.lasertripped === 1 ? 'Door' : 'Window'}</Text>
            <Text>{new Date(post.timestamp).toLocaleString('en-US', { timeZone: 'Asia/Colombo' })}</Text>
            {post.id === lastAlertId && (
              <TouchableOpacity style={styles.offButton} onPress={stopAlarm}>
                <Text style={styles.buttonText}>Off Alarm</Text>
              </TouchableOpacity>
            )}
          </View>
        ))
      )}
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: { flexGrow: 1, padding: 20 },
  heading: { fontSize: 22, fontWeight: 'bold', marginBottom: 10 },
  noData: { fontSize: 16, textAlign: 'center', marginTop: 20, color: 'gray' },
  postContainer: { marginVertical: 10, padding: 10, borderWidth: 1, borderColor: '#ccc', borderRadius: 8 },
  postTitle: { fontSize: 18, fontWeight: 'bold' },
  offButton: { marginTop: 10, padding: 10, backgroundColor: 'red', borderRadius: 5 },
  buttonText: { color: 'white', fontWeight: 'bold' },
});