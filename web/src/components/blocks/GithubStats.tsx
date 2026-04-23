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
    BarChart,
    Bar,
} from "recharts";


function Stat({ label, value }: { label: string; value: any }) {
    return (
        <div className="border border-white/10 p-3 rounded">
            <div className="text-white font-mono">{value}</div>
            <div className="text-white/50 text-xs">{label}</div>
        </div>
    );
}

const CustomTooltip = (valueLabel: string) =>
    function TooltipImpl({ active, payload, label }: any) {
        if (!active || !payload?.length) return null;

        return (
            <div className="bg-[#0b0f1a] border border-white/10 rounded-md px-3 py-2 shadow-lg">
                <div className="text-xs text-white/50 mb-1">
                    {label}
                </div>

                <div className="text-sm text-cyan-300 font-mono">
                    {payload[0].value} {valueLabel}
                </div>
            </div>
        );
    };

function formatYears(date: string) {
    const years =
        (Date.now() - new Date(date).getTime()) /
        (1000 * 60 * 60 * 24 * 365);

    return `${Math.floor(years)} years ago`;
}

export default function GithubStats() {
    const [data, setData] = useState<any>(null);

    const [languageLimit, setLanguageLimit] = useState(10);

    useEffect(() => {
        fetch("/api/github/stats")
            .then((r) => r.json())
            .then(setData);
    }, []);

    const heatmapData = useMemo(() => {
        if (!data?.daily) return [];
        data?.daily.map((d: any) => ({
            date: d.date,
            count: d.count,
        }));
    }, [data?.daily]);

    if (!data) return <Section title="github">loading...</Section>;

    const daily = data.daily;
    const languages = data.languages.slice(0, languageLimit);
    const hasMoreLanguages = languageLimit < data.languages.length;

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
                            <Tooltip content={CustomTooltip("contributions")} />
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
                <div className="grid grid-cols-2 sm:grid-cols-3 md:grid-cols-4 gap-4 text-sm">
                    <Stat
                        label="joined"
                        value={formatYears(data.joined_at)}
                    />
                    <Stat
                        label="total contributions"
                        value={data.total_contributions}
                    />
                    <Stat
                        label="public repos"
                        value={data.public_repos}
                    />
                    <Stat
                        label="activity rate"
                        value={data.active_days_ratio + "%"}
                    />
                    <Stat
                        label="current streak"
                        value={`${data.current_streak} days`}
                    />
                    <Stat
                        label="longest streak"
                        value={`${data.longest_streak} days`}
                    />
                </div>

                {/* HEATMAP */}
                {/* <div className="p-4 border border-white/10 rounded">
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
                </div> */}

                {/* LANGUAGE CHART */}
                <div className="flex flex-col items-center">
                    <ResponsiveContainer width="100%" height={Math.max(180, languages.length * 28)}>
                        <BarChart data={languages} layout="vertical" >
                            <XAxis type="number" />
                            <YAxis dataKey="name" type="category" width={120} interval={0} />
                            <Tooltip content={CustomTooltip("bytes")} />
                            <Bar dataKey="bytes" fill="#22d3ee" barSize={12} />
                        </BarChart>
                    </ResponsiveContainer>
                    {hasMoreLanguages && <button
                        onClick={() => setLanguageLimit((l) => Math.min(l + 10, data.languages.length))}
                        className="mt-3 px-3 py-1 w-fit border border-white/10 rounded text-sm text-white/70 hover:bg-white/5 transition">
                        Load more language data
                    </button>}
                </div>

            </div>
        </Section>
    );
}