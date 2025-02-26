// server.js
const express = require('express');
const bodyParser = require('body-parser');
const { initializeApp } = require('firebase/app');
const { getDatabase, ref, push, get } = require('firebase/database');

// Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyD9CBwVfkc4nRtW-8eoVViwwoDaiuj9P20",
  authDomain: "test-7e4f2.firebaseapp.com",
  projectId: "test-7e4f2",
  storageBucket: "test-7e4f2.firebasestorage.app",
  messagingSenderId: "129883723100",
  appId: "1:129883723100:web:7ffe82e5831e689f0f5d99",
  databaseURL: "https://test-7e4f2-default-rtdb.asia-southeast1.firebasedatabase.app/"
};

// Initialize Firebase app and the Realtime Database
const firebaseApp = initializeApp(firebaseConfig);
const db = getDatabase(firebaseApp);

// Set up the Express server
const app = express();
app.use(bodyParser.json());

// POST endpoint to receive data from the GSM module
app.post('/postdata', async (req, res) => {
  // Get the incoming data from the GSM module
  let data = req.body;
  // Add the current timestamp to the object
  data.timestamp = new Date().toISOString();
  console.log('Received data:', data);
  
  try {
    const dataRef = ref(db, 'data');
    await push(dataRef, data);
    res.status(200).send('Data sent to Firebase successfully.');
  } catch (error) {
    console.error('Error sending data to Firebase:', error);
    res.status(500).send('Error sending data to Firebase: ' + error.message);
  }
});

// GET endpoint to retrieve data from Firebase
app.get('/getdata', async (req, res) => {
  try {
    const dataRef = ref(db, 'data');
    const snapshot = await get(dataRef);
    if (snapshot.exists()) {
      res.status(200).json(snapshot.val());
    } else {
      res.status(404).send('No data available.');
    }
  } catch (error) {
    console.error('Error retrieving data from Firebase:', error);
    res.status(500).send('Error retrieving data from Firebase: ' + error.message);
  }
});

// Start the server on port 3000 and bind to all interfaces
const PORT = process.env.PORT || 3000;
app.listen(PORT, "0.0.0.0", () => {
  console.log(`HTTP server running on port ${PORT}`);
});

