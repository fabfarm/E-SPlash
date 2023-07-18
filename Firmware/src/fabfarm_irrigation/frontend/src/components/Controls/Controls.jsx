import { useState, useEffect } from 'react';
import './Controls.css';

const Controls = ({ data, handleScheduleModeChange, handleToggleRelay, handleRelayAutomaticTime }) => {
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
                                <input
                                    type='time'
                                    name='startTime'
                                    className='start-time'
                                    value={relay.times[0].startTime}
                                    onChange={(e) => handleRelayAutomaticTime(e, relay.pin)}
                                />
                                <input
                                    type='number'
                                    name='duration'
                                    className='duration'
                                    value={relay.times[0].duration}
                                    onChange={(e) => handleRelayAutomaticTime(e, relay.pin)}
                                />
                                <button className='set-time-btn'>Set time</button>
                            </div>
                        )}
                    </div>
                ))}
            </div>
        </section>
    );
};

export default Controls;
