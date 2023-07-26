import { useState, useEffect } from 'react';
import './Controls.css';

const Controls = ({
    data,
    handleScheduleModeChange,
    handleToggleRelay,
    scheduleInputs,
    addSchedule,
    removeSchedule,
    modifySchedule,
}) => {
    console.log('Controls data:', data);

    return (
        <section className='controls'>
            <header className='controls-page-header'>
                <h2>Controls</h2>
            </header>

            <div className='controls-container'>
                {data.relays.map((relay) => (
                    <div className='control-box' key={relay.id}>
                        <header>
                            <div className='box-label'>
                                {relay.name} <span className='pin-info'>Pin {relay.pin}</span>
                            </div>
                            <div className='manual-automatic-check-wrapper'>
                                <input
                                    type='checkbox'
                                    checked={relay.isScheduleMode}
                                    onChange={(e) => handleScheduleModeChange(e, relay.id)}
                                />
                            </div>
                        </header>

                        {!relay.isScheduleMode && (
                            <div className='enable-check-wrapper'>
                                <input
                                    type='checkbox'
                                    checked={relay.isEnabled}
                                    onChange={(e) => handleToggleRelay(e, relay.id)}
                                />
                            </div>
                        )}

                        {!!relay.isScheduleMode && (
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
                                                        <button onClick={() => modifySchedule(relay.id, schedule.id)}>
                                                            Modify
                                                        </button>
                                                        <button onClick={() => removeSchedule(relay.id, schedule.id)}>
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
                                    <input
                                        type='time'
                                        name='startTime'
                                        className='start-time'
                                        value={scheduleInputs.startTime}
                                        onChange={(e) => scheduleInputs.setStartTime(e.target.value)}
                                    />
                                    <input
                                        type='number'
                                        name='duration'
                                        className='duration'
                                        value={scheduleInputs.duration}
                                        onChange={(e) => scheduleInputs.setDuration(e.target.value)}
                                    />
                                    <button className='set-time-btn' onClick={() => addSchedule(relay.id)}>
                                        Add schedule
                                    </button>

                                    {/* A calendar could be done, plus possibility to choose one-time, weekly, monthly. Check how android alarm UI works :) */}
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
