import { useState, useEffect } from 'react';
import './Controls.css';

const Controls = () => {
    return (
        <section className='controls'>
            <header className='controls-page-header'>
                <h2>Controls</h2>
                <div className='main-manual-automatic-wrapper'>
                    <input type='checkbox' />
                </div>
            </header>

            <div className='controls-container'>
                <div className='control-box'>
                    <header>
                        <div className='box-label'>Fruit (pin 27)</div>
                        {/* <div className='manual-auto-wrapper'>
                            <input type='checkbox' />
                        </div> */}
                    </header>

                    <div className='open-close-wrapper'>
                        <input type='checkbox' />
                    </div>

                    <div className='automatic-wrapper'>
                        <input type='time' />
                        <input type='time' />
                        <button>Set time</button>
                    </div>
                </div>

                <div className='control-box'>
                    <header>
                        <div className='box-label'>Vegetables (pin 26)</div>
                        {/* <div className='manual-auto-wrapper'>
                            <input type='checkbox' />
                        </div> */}
                    </header>

                    <div className='open-close-wrapper'>
                        <input type='checkbox' />
                    </div>

                    <div className='automatic-wrapper'>
                        <input type='time' />
                        <input type='time' />
                        <button>Set time</button>
                    </div>
                </div>

                <div className='control-box'>
                    <header>
                        <div className='box-label'>Water (pin 25)</div>
                        {/* <div className='manual-auto-wrapper'>
                            <input type='checkbox' />
                        </div> */}
                    </header>

                    <div className='open-close-wrapper'>
                        <input type='checkbox' />
                    </div>

                    <div className='automatic-wrapper'>
                        <input type='time' />
                        <input type='time' />
                        <button>Set time</button>
                    </div>
                </div>
            </div>
        </section>
    );
};

export default Controls;
