import React, { useState } from 'react';
import './App.css';

function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

function App() {
    const [color, setColor] = useState('#ffffff');
    const [loading, setLoading] = useState(false);

    function handleChange(event) {
        event.persist();
        const hex = event.target.value;
        const rgb = hexToRgb(event.target.value);
        setColor(hex);
        setLoading(true);
        fetch(`http://esp8266.local/update?r=${rgb.r}&g=${rgb.g}&b=${rgb.b}`)
            .then(() => setLoading(false), () => setLoading(false));
    }

    return (
        <div className="app" style={{ backgroundColor: color }}>
            <label className="label">
                <input type="color" ref={(input) => input && input.click()}
                       onChange={handleChange} value={color}/>
            </label>
            {loading && (
                <div className="loader">
                    <div className="loadingio-spinner-dual-ring-707cjr0lre4">
                        <div className="ldio-90lc7mbld2g">
                            <div></div>
                            <div>
                                <div></div>
                            </div>
                        </div>
                    </div>
                </div>
            )}
        </div>
    );
}

export default App;

console.log('App ver: ', process.env.SOURCE_VERSION);