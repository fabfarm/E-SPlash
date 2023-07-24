import './WifiCredentials.css';

const WifiCredentials = ({ updateCredentials }) => {
    // button Change WiFi Credentials, look at updateCredentials

    return (
        <section className='settings-box'>
            <header>
                <h2>Wifi Credentials</h2>
            </header>

            <div>
                <div>
                    {/* label wants name prop? */}
                    <label htmlFor='ssid'>SSID</label>
                    <input type='text' id='ssid' placeholder='ssid' />
                </div>

                <div>
                    <label htmlFor='password'>Password</label>
                    <input type='text' name='password' id='password' placeholder='password' />
                </div>

                <div>
                    <button className='set-wifi-credentials' onClick={updateCredentials}>
                        Set WiFi Credentials
                    </button>
                </div>
            </div>
        </section>
    );
};

export default WifiCredentials;
