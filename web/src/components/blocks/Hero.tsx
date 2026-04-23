import Image from "next/image";
import Section from "../ui/Section";
import { GithubIcon } from "../icons/GithubIcon";
import { LinkedinIcon } from "../icons/LinkedinIcon";

export default function Hero() {
    return (
        <Section title="about_me">
            <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-6">

                {/* avatar */}
                <div className="order-1 sm:order-2 w-full sm:w-auto shrink-0">
                    <Image
                        src="/me.jpg"
                        alt="Jakub"
                        width={120}
                        height={120}
                        className="w-full max-w-[200px] mx-auto sm:mx-0 rounded-full border border-white/10"
                        priority
                    />
                </div>

                {/* text + links */}
                <div className="order-2 sm:order-1 flex flex-col gap-4 max-w-xl">
                    <p className="text-muted-foreground text-center sm:text-left">
                        I'm Jakub, a software developer based in Poland. I like building low-level stuff and weird, but stable systems. I also manage a few servers.
                    </p>

                    {/* socials */}
                    <div className="flex gap-4 justify-center sm:justify-start">
                        <a
                            href="https://github.com/doleckijakub"
                            className="text-white hover:text-cyan-400 transition"
                        >
                            <GithubIcon className="w-8 h-8" />
                        </a>

                        <a
                            href="https://linkedin.com/in/doleckijakub"
                            className="text-white hover:text-cyan-400 transition"
                        >
                            <LinkedinIcon className="w-8 h-8" />
                        </a>
                    </div>
                </div>

            </div>
        </Section>
    );
}