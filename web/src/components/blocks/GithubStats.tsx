"use client";

import { useEffect, useState, useMemo } from "react";
import Section from "../ui/Section";
import CalendarHeatmap from "react-calendar-heatmap";
import "react-calendar-heatmap/dist/styles.css";

import {
    LineChart,
    Line,
    XAxis,
    YAxis,
    Tooltip,
    ResponsiveContainer,
} from "recharts";

function Stat({ label, value }: { label: string; value: any }) {
    return (
        <div className="border border-white/10 p-3 rounded">
            <div className="text-white font-mono">{value}</div>
            <div className="text-white/50 text-xs">{label}</div>
        </div>
    );
}

function formatYears(date: string) {
    const years =
        (Date.now() - new Date(date).getTime()) /
        (1000 * 60 * 60 * 24 * 365);

    return `${Math.floor(years)} years ago`;
}

export default function GithubStats() {
    const [data, setData] = useState<any>(null);

    useEffect(() => {
        fetch("/api/github/stats")
            .then((r) => r.json())
            .then(setData);
    }, []);

    const heatmapData = useMemo(
        () =>
            data?.daily.map((d: any) => ({
                date: d.date,
                count: d.count,
            })),
        [data?.daily]
    );

    if (!data) return <Section title="github">loading...</Section>;

    const daily = data.daily;

    return (
        <Section title="github">
            <div className="space-y-6">

                {/* CONTRIBUTION GRAPH */}
                <div className="p-4 border border-white/10 rounded">
                    <div className="text-xs text-white/50 mb-2">
                        contributions (last 365 days)
                    </div>

                    <ResponsiveContainer width="100%" height={200}>
                        <LineChart data={daily}>
                            <XAxis dataKey="date" hide />
                            <YAxis hide />
                            <Tooltip
                                contentStyle={{
                                    backgroundColor: "#0b0f1a",
                                    border: "1px solid rgba(255,255,255,0.1)",
                                }}
                            />
                            <Line
                                type="monotone"
                                dataKey="count"
                                stroke="#22d3ee"
                                dot={false}
                                strokeWidth={2}
                            />
                        </LineChart>
                    </ResponsiveContainer>
                </div>

                {/* STATS */}
                <div className="grid grid-cols-2 sm:grid-cols-4 gap-4 text-sm">
                    <Stat
                        label="total contributions"
                        value={data.total_contributions}
                    />
                    <Stat
                        label="public repos"
                        value={data.public_repos}
                    />
                    <Stat
                        label="joined"
                        value={formatYears(data.joined_at)}
                    />
                    <Stat
                        label="current streak"
                        value={`${data.current_streak} days`}
                    />
                </div>

                {/* HEATMAP */}
                <div className="p-4 border border-white/10 rounded">
                    <CalendarHeatmap
                        startDate={
                            new Date(Date.now() - 365 * 24 * 60 * 60 * 1000)
                        }
                        endDate={new Date()}
                        values={heatmapData}
                        classForValue={(v: any) => {
                            if (!v) return "color-empty";
                            return `color-scale-${Math.min(v.count, 4)}`;
                        }}
                    />
                </div>

            </div>
        </Section>
    );
}