import './DateAndTime.css';

const DateAndTime = ({ currentTime, inputDate, setInputDate, inputTime, setInputTime, setDateTime }) => {
    // button "Change time and date", look at updateTime
    // input useInternetTime, look at enableInternetUpdate

    return (
        <section className='settings-box'>
            <header>
                <h2>Date and Time</h2>
            </header>

            <div>{currentTime}</div>

            <div className='settings-container'>
                <div>
                    <label htmlFor='date'>Date</label>
                    <input type='date' id='date' name='date' value={inputDate} onChange={setInputDate} />
                </div>
                <div>
                    <label htmlFor='time'>Time</label>
                    <input type='time' id='time' name='time' value={inputTime} onChange={setInputTime} />
                </div>
                <button className='set-date-time-btn' onClick={setDateTime}>
                    Set time and date
                </button>
            </div>
        </section>
    );
};

export default DateAndTime;
