// Firebaseconfiguration.js
import { initializeApp } from 'firebase/app';
import { getDatabase } from 'firebase/database';

const firebaseConfig = {
    apiKey: "AIzaSyD9CBwVfkc4nRtW-8eoVViwwoDaiuj9P20",
    authDomain: "test-7e4f2.firebaseapp.com",
    projectId: "test-7e4f2",
    storageBucket: "test-7e4f2.firebasestorage.app",
    messagingSenderId: "129883723100",
    appId: "1:129883723100:web:7ffe82e5831e689f0f5d99",
    databaseURL: "https://test-7e4f2-default-rtdb.asia-southeast1.firebasedatabase.app/"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

export { db };
