import { useState, useEffect } from 'react';
import './Controls.css';

const Controls = ({
    data,
    handleScheduleModeChange,
    handleToggleRelay,
    handleSchedulingInputsChange,
    postRelayTime,
    removeTime,
    addTime,
}) => {
    console.log('Controls data:', data);

    return (
        <section className='controls'>
            <header className='controls-page-header'>
                <h2>Controls</h2>
                <div className='main-manual-automatic-wrapper'>
                    <input
                        type='checkbox'
                        checked={data.data?.isScheduleMode}
                        onChange={(e) => handleScheduleModeChange(e)}
                    />
                </div>
            </header>

            <div className='controls-container'>
                {data.relays.map((relay, relayIndex) => (
                    <div className='control-box' key={relayIndex}>
                        <header>
                            <div className='box-label'>
                                {relay.name} <span className='pin-info'>Pin {relay.pin}</span>
                            </div>
                        </header>

                        {!data.data.isScheduleMode && (
                            <div className='open-close-wrapper'>
                                <input
                                    type='checkbox'
                                    checked={relay.isEnabled}
                                    onChange={(e) => handleToggleRelay(e, relayIndex)}
                                />
                            </div>
                        )}

                        {!!data.data.isScheduleMode && (
                            <div className='scheduling-wrapper'>
                                {relay.times.map((schedule, scheduleIndex) => (
                                    <div className='inputs-action-wrapper' key={scheduleIndex}>
                                        <input
                                            type='time'
                                            name='startTime'
                                            className='start-time'
                                            value={schedule.startTime}
                                            onChange={(e) => handleSchedulingInputsChange(e, relayIndex, scheduleIndex)}
                                        />
                                        <input
                                            type='number'
                                            name='duration'
                                            className='duration'
                                            value={schedule.duration}
                                            onChange={(e) => handleSchedulingInputsChange(e, relayIndex, scheduleIndex)}
                                        />
                                        <button
                                            className='set-time-btn'
                                            onClick={() => postRelayTime(relayIndex, scheduleIndex)}
                                        >
                                            Set time
                                        </button>
                                        {/* <button className='cancel-time-btn'>Cancel</button> */}
                                        <button
                                            className='remove-schedule-btn'
                                            onClick={() => removeTime(relayIndex, scheduleIndex)}
                                        >
                                            Remove
                                        </button>
                                        {/* <span className='schedule-current-status'></span> */}
                                    </div>
                                ))}

                                <div>
                                    <button className='add-schedule-btn' onClick={() => addTime(relayIndex)}>
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
