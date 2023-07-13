import { Routes, Route } from 'react-router-dom';

import './App.css';

import Topbar from './components/Topbar';
import Home from './pages/Home';
import Settings from './pages/Settings';

function App() {
    return (
        <div className='app-container'>
            <Topbar />
            <Routes>
                <Route path='/' element={<Home />} />
                <Route path='/settings' element={<Settings />} />
            </Routes>
        </div>
    );
}

export default App;
