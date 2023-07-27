import './WifiCredentials.css';

const WifiCredentials = ({ ssidInput, setSsidInput, passwordInput, setPasswordInput, updateCredentials }) => {
    return (
        <section className='settings-box'>
            <header>
                <h2>Wifi Credentials</h2>
            </header>

            <div>
                <div>
                    {/* label wants name prop? */}
                    <label htmlFor='ssid'>SSID</label>
                    <input
                        type='text'
                        id='ssid'
                        placeholder='ssid'
                        value={ssidInput}
                        onChange={(e) => setSsidInput(e.target.value)}
                    />
                </div>

                <div>
                    <label htmlFor='password'>Password</label>
                    <input
                        type='text'
                        name='password'
                        id='password'
                        placeholder='password'
                        value={passwordInput}
                        onChange={(e) => setPasswordInput(e.target.value)}
                    />
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
