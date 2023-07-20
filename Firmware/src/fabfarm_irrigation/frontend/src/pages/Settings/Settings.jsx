import { useState, useEffect } from 'react';

import DateAndTime from '../../components/DateAndTime';
import Relays from '../../components/Relays';
import WifiCredentials from '../../components/WifiCredentials';
import FirmwareUpdate from '../../components/FirmwareUpdate';

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
                    <FirmwareUpdate />
                </>
            )}
        </main>
    );
};

export default Settings;
