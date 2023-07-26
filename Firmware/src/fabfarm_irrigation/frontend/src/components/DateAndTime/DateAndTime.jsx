import './DateAndTime.css';

const DateAndTime = ({ updateTime, enableInternetUpdate }) => {
    // button "Change time and date", look at updateTime
    // input useInternetTime, look at enableInternetUpdate

    return (
        <section className='settings-box'>
            <header>
                <h2>Date and Time</h2>
            </header>

            <div>{'test date and time'}</div>

            <div className='settings-container'>
                <div>
                    <label htmlFor='date'>Date</label>
                    <input type='date' id='date' name='date' />
                </div>
                <div>
                    <label htmlFor='time'>Time</label>
                    <input type='time' id='time' name='time' />
                </div>
                <button className='set-date-time-btn' onClick={updateTime}>
                    Set time and date
                </button>

                {/* <div>
                    <label htmlFor='useInternetTime'>Use internet time</label>
                    <input type='checkbox' id='useInternetTime' onClick={(e) => enableInternetUpdate(e)} />
                </div> */}
            </div>
        </section>
    );
};

export default DateAndTime;
