const FirmwareUpdate = ({ updateFirmware }) => {
    return (
        <section className='settings-box'>
            <header>
                <h2>Firmare Update</h2>
            </header>

            <div>
                <button onClick={updateFirmware}>Update firmare</button>
            </div>
        </section>
    );
};

export default FirmwareUpdate;
