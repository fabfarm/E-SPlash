import React from 'react';
import ReactDOM from 'react-dom/client';
import { BrowserRouter as Router } from 'react-router-dom';

import App from './App.jsx';
import { DataContextProvider } from './context/DataContext';

// import './styles/normalize.css';
import './index.css';

ReactDOM.createRoot(document.getElementById('root')).render(
    <React.StrictMode>
        <Router>
            <DataContextProvider>
                <App />
            </DataContextProvider>
        </Router>
    </React.StrictMode>
);
