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
                            <div className='scheduling-wrapper'>
                                <div className='schedules-table-wrapper'>
                                    <table>
                                        <thead>
                                            <tr>
                                                <th>pos</th>
                                                <th>starttime</th>
                                                <th>duration</th>
                                                <th>action</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            {relay.schedules.map((schedule, index) => (
                                                <tr key={schedule.id}>
                                                    <td>{index}</td>
                                                    <td>{schedule.startTime}</td>
                                                    <td>{schedule.duration}</td>
                                                    <td>
                                                        {/* modify should use same modal as add schedule */}
                                                        <button onClick={() => removeRelaySchedule(schedule.id)}>
                                                            Modify
                                                        </button>
                                                        <button onClick={() => removeRelaySchedule(schedule.id)}>
                                                            Remove
                                                        </button>
                                                    </td>
                                                </tr>
                                            ))}
                                        </tbody>
                                    </table>
                                </div>

                                <div>
                                    {/* use a modal for add a schedule */}
                                    {/* <input
                                        type='time'
                                        name='startTime'
                                        className='start-time'
                                        value={relay.schedules[0].startTime}
                                        onChange={(e) => handleAutoTimeChange(e, relay.pin)}
                                    />
                                    <input
                                        type='number'
                                        name='duration'
                                        className='duration'
                                        value={relay.schedules[0].duration}
                                        onChange={(e) => handleAutoTimeChange(e, relay.pin)}
                                    /> */}
                                    <button className='set-time-btn' onClick={(e) => setAutomaticTime(e)}>
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
