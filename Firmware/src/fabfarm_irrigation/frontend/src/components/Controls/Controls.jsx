import { useState, useEffect } from 'react';
import './Controls.css';

const Controls = ({ data, handleScheduleModeChange }) => {
    return (
        <section className='controls'>
            <header className='controls-page-header'>
                <h2>Controls</h2>
                <div className='main-manual-automatic-wrapper'>
                    <input type='checkbox' checked={data.isScheduleMode} onChange={handleScheduleModeChange} />
                </div>
            </header>

            <div className='controls-container'>
                {data.relays.map((relay, index) => (
                    // use an id instead of the array index in the key prop
                    <div className='control-box' key={index}>
                        <header>
                            <div className='box-label'>
                                {relay.name} <span className='pin-info'>Pin {relay.pin}</span>
                            </div>
                        </header>

                        {!data.isScheduleMode && (
                            <div className='open-close-wrapper'>
                                {/* checked={relay.isEnabled}  */}
                                <input type='checkbox' />
                            </div>
                        )}

                        {!!data.isScheduleMode && (
                            <div className='automatic-wrapper'>
                                <input type='time' />
                                <input type='time' />
                                <button>Set time</button>
                            </div>
                        )}
                    </div>
                ))}
            </div>
        </section>
    );
};

export default Controls;
