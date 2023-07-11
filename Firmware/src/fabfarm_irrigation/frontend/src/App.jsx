import { useState } from 'react';
import './App.css';

import Topbar from './components/Topbar';
import Dashboard from './components/Dashboard';
import Controls from './components/Controls';
import Setup from './components/Setup';

function App() {
    const [count, setCount] = useState(0);

    return (
        <div className='app-container'>
            <Topbar />
            <main>
                <Dashboard />
                <Controls />
                {/* <Setup /> */}
            </main>
        </div>
    );
}

export default App;
