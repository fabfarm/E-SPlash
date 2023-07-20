import './Relays.css';

const Relays = () => {
    // relays: button, look at removeRelay
    // add a relay: 3 inputs, look at addRelay

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
                                <th>Relay name</th>
                                <th>Relay Pin</th>
                                <th>Current status</th>
                                <th>Action</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>test</td>
                                <td>test</td>
                                <td>Off</td>
                                <td>
                                    <button>Remove relay</button>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </section>

                <section>
                    <h3>Add a relay</h3>

                    <div>
                        <input type='text' id='relayname' placeholder='Relay name' />
                        <input type='number' id='relaypin' placeholder='Pin number' />
                        <input type='text' id='relaystatus' placeholder='Relay status' />
                    </div>
                    <button className='add-relay-btn'>Add Relay</button>
                </section>
            </div>
        </section>
    );
};

export default Relays;
