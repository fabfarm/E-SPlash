import { useState } from 'react';
// import reactLogo from './assets/react.svg';
// import viteLogo from '/vite.svg';
import './App.css';

import Dashboard from './components/Dashboard/Dashboard';
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
