import { useState, useEffect } from 'react';

import DateAndTime from '../../components/DateAndTime';
import Relays from '../../components/Relays';
import WifiCredentials from '../../components/WifiCredentials';

const Settings = () => {
    const [isLoading, setIsLoading] = useState(false); // true
    const [error, setError] = useState(null);

    return (
        <main>
            {!isLoading && !error && (
                <>
                    <DateAndTime />
                    <Relays />
                    <WifiCredentials />
                </>
            )}
        </main>
    );
};

export default Settings;
