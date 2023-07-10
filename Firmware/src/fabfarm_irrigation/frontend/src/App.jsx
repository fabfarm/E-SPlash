import { useState } from 'react';
import './App.css';

import Dashboard from './components/Dashboard';
import Controls from './components/Controls/Controls';
import Setup from './components/Setup/Setup';

function App() {
    const [count, setCount] = useState(0);

    return (
        <div>
            <Dashboard />
            <Controls />
            <Setup />
        </div>
    );
}

export default App;
