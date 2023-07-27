import './Relays.css';

const Relays = ({
    relays,
    removeRelay,
    relayNameInput,
    setRelayNameInput,
    relayPinInput,
    setRelayPinInput,
    relayStatusInput,
    setRelayStatusInput,
    addrelay,
}) => {
    return (
        <section className='settings-box'>
            <header>
                <h2>Relays</h2>
            </header>

            <div className='settings-container'>
                <section>
                    <h3>Current relays</h3>

                    <table>
                        <thead>
                            <tr>
                                <th>Relay ID</th>
                                <th>Relay name</th>
                                <th>Relay pin</th>
                                <th>Current status</th>
                                <th>Action</th>
                            </tr>
                        </thead>
                        <tbody>
                            {relays.map((relay) => (
                                <tr key={relay.id}>
                                    <td>{relay.id}</td>
                                    <td>{relay.name}</td>
                                    <td>{relay.pin}</td>
                                    <td>{relay.enabled ? 'On' : 'Off'}</td>
                                    <td>
                                        <button onClick={() => removeRelay(relay.id)}>Remove relay</button>
                                    </td>
                                </tr>
                            ))}
                        </tbody>
                    </table>
                </section>

                <section>
                    <h3>Add a relay</h3>

                    <div>
                        <input
                            type='text'
                            id='relayname'
                            placeholder='Relay name'
                            value={relayNameInput}
                            onChange={(e) => setRelayNameInput(e.target.value)}
                        />
                        <input
                            type='number'
                            id='relaypin'
                            placeholder='Pin number'
                            value={relayPinInput}
                            onChange={(e) => setRelayPinInput(e.target.value)}
                        />
                        {/* <input
                            type='text'
                            id='relaystatus'
                            placeholder='Relay status'
                            value={relayStatusInput}
                            onChange={(e) => setRelayStatusInput(e.target.value)}
                        /> */}
                        <select
                            name='relaystatus'
                            id='relaystatus'
                            style={{ width: '64px' }}
                            value={relayStatusInput}
                            onChange={(e) => setRelayStatusInput(e.target.value)}
                        >
                            {[false, true].map((option, i) => (
                                <option key={i} value={option}>
                                    {option ? 'On' : 'Off'}
                                </option>
                            ))}
                        </select>
                    </div>
                    <button className='add-relay-btn' onClick={addrelay}>
                        Add Relay
                    </button>
                </section>
            </div>
        </section>
    );
};

export default Relays;
