import { useState, useEffect } from 'react';
import './Controls.css';

const Controls = ({ data, handleScheduleModeChange, handleToggleRelay, handleAutoTimeChange, setAutomaticTime }) => {
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
                            <div className='scheduling-wrapper'>
                                {relay.times.map((schedule, index) => (
                                    <div className='inputs-action-wrapper' key={index}>
                                        <input
                                            type='time'
                                            name='startTime'
                                            className='start-time'
                                            value={schedule.startTime}
                                            onChange={(e) => handleAutoTimeChange(e, relay.pin)}
                                        />
                                        <input
                                            type='number'
                                            name='duration'
                                            className='duration'
                                            value={schedule.duration}
                                            onChange={(e) => handleAutoTimeChange(e, relay.pin)}
                                        />
                                        <button className='remove-schedule-btn'>Remove</button>
                                    </div>
                                ))}

                                <div>
                                    <button className='add-schedule-btn' onClick={(e) => setAutomaticTime(e)}>
                                        Add schedule
                                    </button>
                                </div>
                            </div>
                        )}
                    </div>
                ))}
            </div>
        </section>
    );
};

export default Controls;
