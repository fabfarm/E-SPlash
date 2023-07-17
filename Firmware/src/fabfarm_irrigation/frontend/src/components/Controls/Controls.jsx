import { useState, useEffect } from 'react';
import './Controls.css';

const Controls = ({ data, handleScheduleModeChange, handleToggleRelay }) => {
    console.log('Controls data:', data);

    return (
        <section className='controls'>
            <header className='controls-page-header'>
                <h2>Controls</h2>
                <div className='main-manual-automatic-wrapper'>
                    <input type='checkbox' checked={data.isScheduleMode} onChange={handleScheduleModeChange} />
                </div>
            </header>

            <div className='controls-container'>
                {data.relays.map((relay) => (
                    // use an id instead of relay.pin in the key prop
                    <div className='control-box' key={relay.pin}>
                        <header>
                            <div className='box-label'>
                                {relay.name} <span className='pin-info'>Pin {relay.pin}</span>
                            </div>
                        </header>

                        {!data.isScheduleMode && (
                            <div className='open-close-wrapper'>
                                <input
                                    type='checkbox'
                                    checked={relay.isEnabled}
                                    onChange={() => handleToggleRelay(relay.pin)}
                                />
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
