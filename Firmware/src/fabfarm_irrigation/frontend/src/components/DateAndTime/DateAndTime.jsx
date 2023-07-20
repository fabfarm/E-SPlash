import './DateAndTime.css';

const DateAndTime = () => {
    // button "Change time and date", look at updateTime
    // input useInternetTime, look at enableInternetUpdate

    return (
        <section className='date-and-time'>
            <header>
                <h2>Date and Time</h2>
            </header>

            <div>Thursday, July 20 2023 - 17:32 </div>

            <div className='settings-container'>
                <div>
                    <label htmlFor='date'>Date</label>
                    <input type='date' id='date' name='date' />
                </div>
                <div>
                    <label htmlFor='time'>Time</label>
                    <input type='time' id='time' name='time' />
                </div>
                <button className='set-date-time-btn'>Set time and date</button>
            </div>
        </section>
    );
};

export default DateAndTime;
